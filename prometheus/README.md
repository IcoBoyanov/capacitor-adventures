#

## Pre-requisite
You need a docker-compose installed on your machine [install guide](https://docs.docker.com/compose/install/)

## Build and run
```shell
docker-compose up
```

If you need to delete all data run:
```shell
docker-compose rm
```

## General Information
This setup will deploy grafana and prometheus in the same docker network. Both deployments will be available on:
```
http://localhost:3000  # Grafana
http://localhost:9090  # Prometheus
```
Access to grafana:
username: admin
password: root