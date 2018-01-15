tapio: A simple TAP to stdin/stdout proxy.
---
`tapio` is a simple TAP to stdin/stdout proxy.

### Usage

To create a TAP tunnel over ssh:

```
# gcc tapio.c -o  tapio
# mkfifo peer_output
# cat peer_output | ./tapio tap1 mtu 1400 mode tap | ssh user@remote './tapio tap1 mtu 1400 mode tap' > peer_output
```

### License

Public Domain.
