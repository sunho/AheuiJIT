module.exports = {
  webpack5: true,
  webpack: (config) => {
    config.resolve.fallback = { fs: false };
    return config;
  },
  basePath: '/AheuiJIT',
  assetPrefix: '/AheuiJIT/'
}