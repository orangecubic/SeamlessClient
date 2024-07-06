#pragma once

#include <axmol.h>
#include <engine/NetworkEngine.h>

class GameScene;

class ClientWorker : public NetworkEngineWorker
{
private:
    GameScene* _scene;
    NetworkEngine* _engine;

    SocketAddress _agency_address;
    SocketSessionPtr _agency_session;

    uint64_t _agency_connector_id;

    void OnAgencyGameEnterRes(const SocketSessionPtr& session,
                              const PacketHeader& header,
                              const packet_game_enter_request_rs& packet);

    void OnAgencyMoveRes(const SocketSessionPtr& session,
                              const PacketHeader& header,
                              const packet_character_move_rs& packet);

    void OnAgencyUpdateObject(const SocketSessionPtr& session,
                              const PacketHeader& header,
                              const packet_object_list_ps& packet);

    void OnAgencyObjectCreate(const SocketSessionPtr& session,
                              const PacketHeader& header,
                              const packet_create_object_ps& packet);

    void OnAgencyObjectRemove(const SocketSessionPtr& session,
                              const PacketHeader& header,
                              const packet_remove_object_ps& packet);

    void OnAgencyObjectMove(const SocketSessionPtr& session,
                            const PacketHeader& header,
                            const packet_object_move_ps& packet);

public:

    void Initialize(GameScene* const scene, NetworkEngine* const engine, const SocketAddress& agency_address);

    virtual void OnSocketSessionConnected(SocketContext* context) override;
    virtual void OnSocketSessionAbandoned(SocketContext* context) {}
    virtual void OnSocketSessionAlived(SocketContext* context) {}
    virtual void OnSocketSessionClosed(SocketContext* context) override;
    virtual void OnSocketSessionData(SocketContext* context) override;

    virtual bool Update(const WorkerTimeUnit current_time, const WorkerTimeUnit delta_time) override;
};
