# TODO list
small\_watcher etcd driver need a refactoring, to store Async Req in Etcd

# small\_server small\_watcher package relationship refactor

at master c1781505213ce42f9f41b0d3f7c19bba0b3fbab1
```
1 small_watcher -> make_watcher -> etcd -> http_client -> resolver manager
small_watcher -> watcher_resolver(and register watcher_resolver in resolver manager)

2 small_server now include:
grpc_server

grpc_server_grpc_client->grpc_client
grpc_server_http_client->http_client
grpc_server_redis_client->redis_client

grpc_client->client_channel
http_client->client_channel
redis_client->client_channel
```

and now changed to

```
1 small_watcher->http_client 
small_watcher -> watcher_resolver(and register watcher_resolver in resolver manager)
2 small_server->small_watcher:
grpc_server_grpc_client->grpc_client
grpc_server_http_client->http_client
grpc_server_redis_client->redis_client
grpc_client

3 small_clients
http_client:
redis_client:
client_channel:
```
