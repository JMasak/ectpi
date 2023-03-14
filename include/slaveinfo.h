#pragma once
#include <stdint.h>

struct EscInfo
{
    uint16_t    pdiControl;
    uint16_t    pdiConfiguration;
    uint16_t    syncImpulse;
    uint16_t    pdiConfig2;
    uint16_t    stationAlias;
    uint8_t     reserved[4];
    uint16_t    checksum;
};

struct Identity
{
    uint32_t    vendorId;
    uint32_t    productCode;
    uint32_t    revisionNumber;
    uint32_t    serialNumber;
    uint8_t     reserved[8];
};

struct MboxBaseConfig
{
    uint16_t    receiveOffset;
    uint16_t    receiveSize;
    uint16_t    sendOffset;
    uint16_t    sendSize;
};

struct MailboxConfig : MboxBaseConfig
{
    uint16_t    mailboxProtocol;
    uint8_t     reserved[66];
};

struct SiiSize
{
    uint16_t    size;
    uint16_t    version;
};

struct SlaveInfo
{
    EscInfo         escInfo;
    Identity        identity;
    MboxBaseConfig  bootstrapMailbox;
    MailboxConfig   mailbox;
    SiiSize         siiSize;
};