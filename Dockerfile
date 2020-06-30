FROM node:14.4.0-stretch-slim

WORKDIR /usr/src/app
COPY . /usr/src/app/

RUN npm install

RUN groupadd -g 999 appuser && useradd -r -u 999 -g appuser appuser
USER appuser

EXPOSE 8080

ENTRYPOINT exec node ./src/index.js

HEALTHCHECK --interval=10s --timeout=2s --start-period=15s \
    CMD node src/healthcheck.js || exit 1
