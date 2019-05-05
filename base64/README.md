# Base64 util for sqlite3

```bash
sqlite3
.load ./base64
select base64_decode(base64_encode("hello")) # should return "hello"
```
