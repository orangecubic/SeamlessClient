#include "World.h"

inline ax::Vec2 PosConvert(const Vector2 pos)
{
    return ax::Vec2(pos.x, pos.y);
}

inline Vector2 PosConvert(const ax::Vec2 pos)
{
    return Vector2(pos.x, pos.y);
}

ax::Vec2 TransformServerPosToDrawingPos(const Vector2 pos)
{
    constexpr FixtureRectangle center_rect = GetSectorAbsFixtureRectangle(CenterSector.sector_id);
    constexpr Vector2 base_pos = center_rect.LeftDown();

    return ax::Vec2(base_pos.x + pos.x, base_pos.y + pos.y);
}

Vector2 TransformDrawingPosToServerPos(const ax::Vec2 pos)
{
    constexpr FixtureRectangle center_rect = GetSectorAbsFixtureRectangle(CenterSector.sector_id);
    constexpr Vector2 base_pos             = center_rect.LeftDown();

    return Vector2{base_pos.x - pos.x, base_pos.y - pos.y};
}

WorldObject* World::Initialize(ax::Scene* draw_scene, const packet_game_enter_request_rs& packet)
{
    _visible_size = ax::Director::getInstance()->getVisibleSize();
    
    auto origin      = ax::Director::getInstance()->getVisibleOrigin();

    Vector2 world_size = GetSectorAbsFixtureRectangle(SectorGridWidth - 1).RightUp();

    _camera_node = ax::Node::create();
    _camera_node->setContentSize(ax::Vec2(world_size.x, world_size.y));
    _camera_node->setPosition(ax::Vec2::ZERO);

    _sector_border = ax::DrawNode::create();
    _sector_border->setPosition(ax::Vec2::ZERO);

    for (const auto& sector : SectorGridList)
    {
        FixtureRectangle rect = GetSectorFixtureRectangle(sector.sector_id);

        ax::Vec2 start = TransformServerPosToDrawingPos(rect.LeftDown());
        _sector_border->drawRect(start, start + PosConvert(static_cast<Vector2>(SectorSize)), ax::Color4B::GREEN);
    }

    _camera_node->addChild(_sector_border);

    ax::Vec2 start_pos = TransformServerPosToDrawingPos(Vector2{0, 0});
    _world_node = ax::Node::create();
    _world_node->setPosition(start_pos);
    _world_node->setContentSize(ax::Vec2(world_size.x - start_pos.x, world_size.y - start_pos.y));

    _camera_node->addChild(_world_node);

    _me.object = new Object(packet.object_info.object_id, packet.object_info.sector_id, ObjectType::Character,
                            ObjectState::Stop, packet.object_info.vital, packet.object_info.fixture);

    _me.object_node = ax::DrawNode::create();
    _me.object_node->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE);
    _me.object_node->setPosition(PosConvert(packet.object_info.fixture.position));
    _me.object_node->drawSolidRect(ax::Vec2::ZERO, PosConvert(packet.object_info.fixture.size), ax::Color4B::GREEN);
    _me.object_node->setContentSize(PosConvert(packet.object_info.fixture.size));
    _world_node->addChild(_me.object_node);

    draw_scene->addChild(_camera_node);

    return &_me;
}

void World::Cleanup()
{
    if (_me.object == nullptr)
        return;

    delete _me.object;
    _me.object = nullptr;
    _me.object_node->removeFromParent();

    _me.object_node = nullptr;

    for (auto& object : _objects)
    {
        delete object.second.object;
        object.second.object_node->removeFromParent();
    }

    _objects.clear();

    _sector_border->removeFromParent();
    _world_node->removeFromParent();
    _camera_node->removeFromParent();
}

void World::UpdateObject(const packet_object_list_ps& objects)
{
    decltype(_objects) remove_objects = _objects;

    for (const PacketObjectInfo& object_info : objects.object_list)
    {
        if (object_info.object_id == _me.object->GetId())
            continue;

        remove_objects.erase(object_info.object_id);
        UpdateObject(object_info);
    }

    for (const auto& object : remove_objects)
    {
        delete object.second.object;
        object.second.object_node->removeFromParent();

        _objects.erase(object.first);
    }
        
}
void World::UpdateObject(const PacketObjectInfo& object_info)
{
    WorldObject& object = _objects[object_info.object_id];

    if (object.object_node != nullptr)
        return;

    if (object.object_node == nullptr)
    {
        object.object = new Object(object_info.object_id, object_info.sector_id, object_info.type, ObjectState::Stop,
                                   object_info.vital, object_info.fixture);
        object.object_node = ax::DrawNode::create();
        object.object_node->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE);
        object.object_node->setContentSize(PosConvert(object_info.fixture.size));
        _world_node->addChild(object.object_node);
    }

    object.object_node->setPosition(PosConvert(object_info.fixture.position));
    object.object_node->clear();
    object.object_node->drawSolidRect(ax::Vec2::ZERO, PosConvert(object_info.fixture.size), ax::Color4B::WHITE);
    
    *object.object->GetFixture() = object_info.fixture;
    *object.object->GetVital() = object_info.vital;
}

void World::MoveObject(const packet_object_move_ps& packet)
{
    if (_me.object == nullptr)
        return;

    if (packet.object_id == _me.object->GetId())
        return;

    WorldObject& object = _objects[packet.object_id];

    if (object.object == nullptr)
    {
        _objects.erase(packet.object_id);
        return;
    }

    object.object->GetFixture()->position  = packet.starting_position;
    object.object->GetFixture()->direction = packet.direction;

    AXLOG("sector %llu object %llu position, %f, %f", packet.sector_id, packet.object_id, packet.starting_position.x, packet.starting_position.y);
    object.object_node->setPosition(PosConvert(packet.starting_position));
}
void World::Move(const Direction direction)
{

}

void World::RemoveObject(const packet_remove_object_ps& packet)
{
    auto object_iterator = _objects.find(packet.object_id);
    if (object_iterator == _objects.end())
        return;


    delete object_iterator->second.object;
    object_iterator->second.object_node->removeFromParent();

    _objects.erase(object_iterator);
    
}

void World::CreateObject(const packet_create_object_ps& packet)
{
    UpdateObject(packet.object_info);
}

void World::Update(float delta)
{
    if (_me.object == nullptr)
        return;

    for (auto& object : _objects)
    {
        Vector2 movement = object.second.object->GetFixture()->Move(utility::Milliseconds(static_cast<int32_t>(delta*1000)));
        object.second.object_node->setPosition(object.second.object_node->getPosition() + PosConvert(movement));
    }

    Vector2 movement = _me.object->GetFixture()->Move(utility::Milliseconds(static_cast<int32_t>(delta*1000)));

    _me.object_node->setPosition(_me.object_node->getPosition() + PosConvert(movement));

    ax::Vec2 view_position = _me.object_node->getWorldPosition();

    ax::Vec2 desired_position = view_position;

    if (desired_position.x > _visible_size.x * (1 - CAMERA_BOX_BORDER_RATIO))
        desired_position.x = _visible_size.x * (1 - CAMERA_BOX_BORDER_RATIO);
    else if (desired_position.x < _visible_size.x * CAMERA_BOX_BORDER_RATIO)
        desired_position.x = _visible_size.x * CAMERA_BOX_BORDER_RATIO;

    if (desired_position.y > _visible_size.y * (1 - CAMERA_BOX_BORDER_RATIO))
        desired_position.y = _visible_size.y * (1 - CAMERA_BOX_BORDER_RATIO);
    else if (desired_position.y < _visible_size.y * CAMERA_BOX_BORDER_RATIO)
        desired_position.y = _visible_size.y * CAMERA_BOX_BORDER_RATIO;

    ax::Vec2 camera_alignment = ax::Vec2(view_position.x - desired_position.x, view_position.y - desired_position.y);

    ax::Vec2 camera_pos = _camera_node->getPosition();

    camera_pos.x -= camera_alignment.x;
    camera_pos.y -= camera_alignment.y;

    _camera_node->setPosition(camera_pos);
}
