tapio: A simple TAP to stdin/stdout proxy.
---
`tapio` is a simple TAP to stdin/stdout proxy.

### Usage

```
# gcc tapio.c -o tapio
# mkfifo peer_output
# cat peer_output | ./tapio tap1 1400 | ssh user@remote './tapio tap1 1400' > peer_output
```

### License

Public Domain.
