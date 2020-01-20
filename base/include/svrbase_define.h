
/*******************************************************************
** File Name:       svrbase_define.h
** Copy Right:      neil deng
** File creator:    neil deng
** Date:            
** Version:
** Desc:            
********************************************************************/

#ifndef _SVRBASE_DEFINE_HY
#define _SVRBASE_DEFINE_HY

#include "base_define.h"
/*-------------------------------------------------------------------*/
#define MAX_SVRNAME_LEN         32
#define MAX_SVRIP_LEN           128
#define MAX_SVRTYPE_COUNT       32
#define MAX_SVRCOUNT_PERTYPE    32
#define MAX_GAMESIG_LEN         128

typedef struct _tagSvrBaseDef_
{
    char m_cSvrType;
    char m_szSvrName[MAX_SVRNAME_LEN];
    int m_iSvrID;
    char m_szListenIP[MAX_SVRIP_LEN];
    int m_iSvrPort;
    char m_szInternalIP[MAX_SVRIP_LEN];
} TSvrDefine;

/*-------------------------------------------------------------------*/
enum MSG_TYPE_ENUM
{
    msg_type_request = 1,
    msg_type_response,
    msg_type_notify,
};

enum SERVER_ENTITY_TYPE_ENUM
{
    entity_type_min         = 0,
    entity_type_client      = 1,
    entity_type_game_svr    = 2,
    entity_type_gate_svr    = 3,
    entity_type_cache_svr   = 4,
    entity_type_center_svr  = 5,
    entity_type_login_svr   = 6,
    entity_type_control_svr = 7,
    entity_type_db_svr      = 8,
    entity_type_robot_svr   = 9,
    entity_type_max
};

enum enCloseReason
{
    enCloseReason_clientrst                  = 100,
    enCloseReason_nowrapper                  = 101,
    enCloseReason_clientclose                = 102,
    enCloseReason_clientpacket_err           = 103,
    enCloseReason_svrclose                   = 104,
    enCloseReason_svr_packet_err             = 105,
    enCloseReason_2svr_fail                  = 106,
    enCloseReason_clientsocket_timeout       = 107,
    enCloseReason_send2client_fail           = 108,
    enCloseReason_svr_killclient             = 109,
    enCloseReason_up_toomuch                 = 110,
    enCloseReason_recv_buf_full              = 111,
    enCloseReason_interrupted                = 112,
    enCloseReason_sock_err                   = 113,
    enCloseReason_client_send_outsize        = 114,    //客户端发送了过大的包过来
    enCloseReason_client_packet_invaild      = 115,  //客户端发来了非发尝试包
};


#endif