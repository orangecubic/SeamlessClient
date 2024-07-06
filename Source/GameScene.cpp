#include "GameScene.h"
#include <sstream>

USING_NS_AX;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "GameScene.cpp\n");
}

Direction GameScene::CalculateDirection()
{
    if (_key_enabled.size() > 2 || _key_enabled.empty())
        return Direction::Max;

    if (_key_enabled.size() == 1)
        return *_key_enabled.begin();

    Direction first_direction = *_key_enabled.begin();
    Direction second_direction = *(++_key_enabled.begin());

    int32_t key_distance = static_cast<int>(first_direction) - static_cast<int>(second_direction);
    if (std::abs(key_distance) != 2)
    {
        if (std::abs(key_distance) == 6)
            return Direction::UpLeft;

        return Direction::Max;
    }

    Direction small_direction = std::min(first_direction, second_direction);

    return static_cast<Direction>(static_cast<int32_t>(small_direction) + 1);
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();
    auto safeArea    = _director->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;

    _black_screen = ax::Node::create();

    _black_screen->setPosition(ax::Vec2::ZERO);

    auto draw_node = ax::DrawNode::create();
    draw_node->setPosition(Vec2::ZERO);
    draw_node->drawSolidRect(ax::Vec2::ZERO, visibleSize, ax::Color4B::BLACK);

    _black_screen->addChild(draw_node);

    auto text_node = Label::createWithTTF("Disconnected from server", "fonts/arial.ttf", 24);
    text_node->setPosition(visibleSize / 2);
    text_node->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE);
    text_node->setColor(ax::Color3B::WHITE);
    _black_screen->addChild(text_node);
    _black_screen->retain();

    addChild(_black_screen, 999999999 + 1);

    _pos_label = Label::createWithTTF("x: 0, y: 0", "fonts/arial.ttf", 24);
    _pos_label->setPosition(ax::Vec2(10, visibleSize.y - 10));
    _pos_label->setAnchorPoint(ax::Vec2::ANCHOR_TOP_LEFT);

    addChild(_pos_label, 999999999);

    _world = new World();

    auto keyboardListener           = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed  = AX_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = AX_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 11);

    scheduleUpdate();
    return true;
}

thread_local std::stringstream string_stream;

void GameScene::update(float delta)
{
    if (!_started)
        return;

    _world->Update(delta);

    string_stream << "x: " << _me->object->GetFixture()->position.x << ", y: " << _me->object->GetFixture()->position.y;
    
    _pos_label->setString(string_stream.str());
    string_stream.str("");

}

void GameScene::StartGame(const SocketSessionPtr& session, const packet_game_enter_request_rs& packet)
{
    _me = _world->Initialize(this, packet);
    _black_screen->removeFromParent();
    _server_session = session;
    _started = true;
}

void GameScene::StopGame()
{
    if (!_started)
        return;

    _me = nullptr;
    _world->Cleanup();
    addChild(_black_screen);
    _started = false;
}

void GameScene::ObjectMove(const packet_object_move_ps& packet)
{
    _world->MoveObject(packet);
}
    
void GameScene::ObjectCreate(const packet_create_object_ps& packet)
{
    _world->CreateObject(packet);
}

void GameScene::ObjectRemove(const packet_remove_object_ps& packet)
{
    _world->RemoveObject(packet);
}

void GameScene::UpdateObjectList(const packet_object_list_ps& packet)
{
    _world->UpdateObject(packet);
}

void GameScene::MoveResponse(const packet_character_move_rs& packet)
{

}

// Keyboard
void GameScene::onKeyPressed(ax::EventKeyboard::KeyCode code, ax::Event* event)
{
    if (!_started)
        return;

    Direction pushed_direction = Direction::Max;

    if (code == ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
        pushed_direction = Direction::Left;
    }
    else if (code == ax::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
        pushed_direction = Direction::Up;
    }
    else if (code == ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
        pushed_direction = Direction::Right;
    }
    else if (code == ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
        pushed_direction = Direction::Down;
    }

    if (pushed_direction == Direction::Max)
        return;

    if (_key_enabled.find(pushed_direction) != _key_enabled.end())
        return;

    _key_enabled.insert(pushed_direction);

    Direction current_direction = CalculateDirection();

    _me->object->GetFixture()->direction = current_direction;

    packet_character_move_rq packet;

    packet.direction = current_direction;
    packet.object_id = _me->object->GetId();
    packet.starting_position = _me->object->GetFixture()->position;

    _server_session->Send(packet);
}

void GameScene::onKeyReleased(ax::EventKeyboard::KeyCode code, ax::Event* event)
{
    if (!_started)
        return;

    if (code == ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
        _key_enabled.erase(Direction::Left);
    }
    else if (code == ax::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
        _key_enabled.erase(Direction::Up);
    }
    else if (code == ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
        _key_enabled.erase(Direction::Right);
    }
    else if (code == ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
        _key_enabled.erase(Direction::Down);
    }

    Direction current_direction = CalculateDirection();

    _me->object->GetFixture()->direction = current_direction;

    packet_character_move_rq packet;

    packet.direction         = current_direction;
    packet.object_id         = _me->object->GetId();
    packet.starting_position = _me->object->GetFixture()->position;

    _server_session->Send(packet);
}
