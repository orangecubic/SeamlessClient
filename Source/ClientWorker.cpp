#include "ClientWorker.h"
#include "GameScene.h"
#include <axmol.h>

#define CLIENT_PACKET_CASE(packet_type, handler)                           \
    case PacketType::##packet_type##:                                        \
    {                                                                        \
        packet_type packet;                                                  \
        if (!DeserializePacketBody(context->buffer.Data(), context->header.body_size, packet)) \
        {                                                                    \
            AXLOG("falied to deserialize packet %s", typeid(packet_type).name());         \
            return;                                                          \
        }                                                                    \
        handler##(context->session, context->header, packet);                                  \
        break;                                                               \
    }

void ClientWorker::Initialize(GameScene* const scene, NetworkEngine* const engine, const SocketAddress& agency_address)
{
    _scene = scene;
    _engine = engine;
    _agency_address = agency_address;
    
    _agency_connector_id = _engine->RegisterConnectorSocket(agency_address, 0);
}

void ClientWorker::OnSocketSessionConnected(SocketContext* context)
{

}

void ClientWorker::OnSocketSessionClosed(SocketContext* context)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->StopGame(); });
}

void ClientWorker::OnSocketSessionData(SocketContext* context)
{

    switch (context->header.packet_type)
    {
        CLIENT_PACKET_CASE(packet_game_enter_request_rs, OnAgencyGameEnterRes);
        CLIENT_PACKET_CASE(packet_character_move_rs, OnAgencyMoveRes);
        CLIENT_PACKET_CASE(packet_object_list_ps, OnAgencyUpdateObject);
        CLIENT_PACKET_CASE(packet_create_object_ps, OnAgencyObjectCreate);
        CLIENT_PACKET_CASE(packet_remove_object_ps, OnAgencyObjectRemove);
        CLIENT_PACKET_CASE(packet_object_move_ps, OnAgencyObjectMove);
    }
}

void ClientWorker::OnAgencyGameEnterRes(const SocketSessionPtr& session,
    const PacketHeader& header,
    const packet_game_enter_request_rs& packet)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->StartGame(session, packet); });
}

void ClientWorker::OnAgencyMoveRes(const SocketSessionPtr& session,
    const PacketHeader& header,
    const packet_character_move_rs& packet)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->MoveResponse(packet); });
}

void ClientWorker::OnAgencyUpdateObject(const SocketSessionPtr& session,
    const PacketHeader& header,
    const packet_object_list_ps& packet)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->UpdateObjectList(packet); });
}

void ClientWorker::OnAgencyObjectCreate(const SocketSessionPtr& session,
    const PacketHeader& header,
    const packet_create_object_ps& packet)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->ObjectCreate(packet); });
}

void ClientWorker::OnAgencyObjectRemove(const SocketSessionPtr& session,
    const PacketHeader& header,
    const packet_remove_object_ps& packet)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->ObjectRemove(packet); });
}

void ClientWorker::OnAgencyObjectMove(const SocketSessionPtr& session,
    const PacketHeader& header,
    const packet_object_move_ps& packet)
{
    _scene->getScheduler()->runOnAxmolThread([=]() { this->_scene->ObjectMove(packet); });
}

bool ClientWorker::Update(const WorkerTimeUnit current_time, const WorkerTimeUnit delta_time)
{
    return true;
}
