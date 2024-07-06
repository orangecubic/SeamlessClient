#pragma once

#include <axmol.h>
#include <game/coordinate/SectorGrid.h>
#include <game/object/Object.h>
#include <engine/protocol/Packet.h>

struct WorldObject
{
    ::Object* object;
    ax::DrawNode* object_node;
};

class World
{
private:
    ax::Vec2 _visible_size;
    std::unordered_map<uint64_t, WorldObject> _objects;

    ax::Scene* _draw_scene;

    // camera node의 좌표를 이동시켜 화면 Draw
    ax::Node* _camera_node;

    // world node의 좌표는 고정
    ax::Node* _world_node;

    // sector 경계를 그리는 node
    ax::DrawNode* _sector_border;
    WorldObject _me;


    const CoordinateUnit CAMERA_BOX_BORDER_RATIO = 0.2f;

    void UpdateObject(const PacketObjectInfo& object_info);

public:
    WorldObject* Initialize(ax::Scene* draw_scene, const packet_game_enter_request_rs& packet);
    void Cleanup();

    void UpdateObject(const packet_object_list_ps& objects);
    void MoveObject(const packet_object_move_ps& packet);
    void Move(const Direction direction);

    void RemoveObject(const packet_remove_object_ps& packet);
    void CreateObject(const packet_create_object_ps& packet);
    void Update(float delta);
};
