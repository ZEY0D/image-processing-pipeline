const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function (app) {
  app.use(
    createProxyMiddleware({
      pathFilter: '/process',
      target: 'http://127.0.0.1:18080',
      changeOrigin: true,
    })
  );
};
