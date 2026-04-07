#pragma once
#include <stdint.h>
#include <sys/socket.h>
struct sockaddr_can { sa_family_t can_family; int can_ifindex; };
struct can_frame { uint32_t can_id; uint8_t can_dlc; uint8_t __pad; uint8_t __res0; uint8_t len8_dlc; uint8_t data[8]; };
struct canfd_frame { uint32_t can_id; uint8_t len; uint8_t flags; uint8_t __res0; uint8_t __res1; uint8_t data[64]; };
#define CAN_RAW 1
#define CAN_MTU sizeof(struct can_frame)
#define CANFD_MTU sizeof(struct canfd_frame)
#define SOL_CAN_RAW 100
#define CAN_RAW_FD_FRAMES 1
#define CAN_RAW_FILTER 1
#define PF_CAN 29
#define AF_CAN PF_CAN
