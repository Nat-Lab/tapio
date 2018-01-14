// tapio: tap to stdin/stdout (c) 2018 magicnat

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_ether.h>
#include <netinet/in.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s IFNAME [LINK_MTU]\n", argv[0]);
    exit(1);
  }

  int TBufLen = argc > 2 ? atoi(argv[2]) : ETH_FRAME_LEN,
      Tfd, RBufLen = 0, WBufLen = 0, PLen;

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));

  strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
  ifr.ifr_flags = IFF_TAP;

  Tfd = open("/dev/net/tun", O_RDWR);
  if (ioctl(Tfd, TUNSETIFF, (void *)&ifr) < 0) {
    fprintf(stderr, "[ERR] TUNSETIFF failed.\n");
    exit(1);
  }

  ifr.ifr_mtu = TBufLen - 18;
  if (ioctl(socket(AF_INET, SOCK_STREAM, IPPROTO_IP), SIOCSIFMTU, (void *)&ifr) < 0)
    fprintf(stderr, "[WARN] SIOCSIFMTU failed, please set MTU of %s to %d manually.\n", ifr.ifr_name, TBufLen - 18);

  fd_set RFdSet, WFdSet;
  unsigned char RBuf[TBufLen], WBuf[TBufLen];

  fprintf(stderr, "[INFO] Attached to %s, Frame length %d.\n", argv[1], TBufLen);

  do {
    FD_ZERO(&RFdSet);
    if (RBufLen <= 0) FD_SET(Tfd, &RFdSet);
    if (WBufLen <= 0) FD_SET(0, &RFdSet);

    FD_ZERO(&WFdSet);
    if (WBufLen > 0) FD_SET(Tfd, &WFdSet);
    if (RBufLen > 0) FD_SET(1, &WFdSet);

    if(select(Tfd + 1, &RFdSet, &WFdSet, NULL, NULL) < 0) continue;

    if (FD_ISSET(Tfd, &RFdSet) && RBufLen == 0)
      RBufLen = read(Tfd, RBuf, TBufLen);

    if (FD_ISSET(Tfd, &WFdSet) && WBufLen > 0) {
      write(Tfd, WBuf, WBufLen);
      WBufLen = 0;
    }

    if (FD_ISSET(0, &RFdSet) && WBufLen == 0) {
      WBufLen = 0;
      read(0, &PLen, 4);
      while(WBufLen < PLen) {
        WBufLen += read(0, WBuf, PLen - WBufLen);
      }
    }

    if (FD_ISSET(1, &WFdSet) && RBufLen > 0) {
      write(1, &RBufLen, 4);
      write(1, RBuf, RBufLen);
      RBufLen = 0;
    }
  } while (1);
}
