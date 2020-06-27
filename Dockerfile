FROM  bitfinex-c-gateway-builder:latest AS builder

WORKDIR /usr/src/app
COPY . /usr/src/app/

WORKDIR /usr/src/app/build
RUN cmake -DENABLE_COVERAGE=0 -DCMAKE_BUILD_TYPE=Release .. && make

FROM debian:buster-20200607-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
		ca-certificates \
		curl \
		openssl \
		zlib1g \
	&& rm -rf /var/lib/apt/lists/*

RUN groupadd -g 999 appuser && useradd -r -u 999 -g appuser appuser
USER appuser

EXPOSE 7681

ENTRYPOINT exec ./bitfinex-gateway

HEALTHCHECK --start-period=5s \
  CMD curl --silent --fail --max-time 30 http://localhost:7681/ping.html || exit 1

COPY --from=builder /usr/src/app/build/src/bitfinex-gateway .
COPY --from=builder /usr/src/app/build/src/mount-origin ./mount-origin