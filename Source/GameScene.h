#pragma once

#include "axmol.h"
#include "World.h"
#include <engine/protocol/Packet.h>
#include <engine/NetworkEngine.h>

class GameScene : public ax::Scene
{
private:
    SocketSessionPtr _server_session;
    World* _world = nullptr;
    WorldObject* _me;
    ax::Label* _pos_label;

    std::unordered_set<Direction> _key_enabled;

    ax::Node* _black_screen;
    bool _started = false;

    Direction CalculateDirection();

public:
    bool init() override;
    void update(float delta) override;

    void StartGame(const SocketSessionPtr& session, const packet_game_enter_request_rs& packet);
    void StopGame();

    void MoveResponse(const packet_character_move_rs& packet);

    void ObjectMove(const packet_object_move_ps& packet);
    void ObjectCreate(const packet_create_object_ps& packet);
    void ObjectRemove(const packet_remove_object_ps& packet);

    void UpdateObjectList(const packet_object_list_ps& packet);

    

    
    // Keyboard
    void onKeyPressed(ax::EventKeyboard::KeyCode code, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode code, ax::Event* event);

private:

};
