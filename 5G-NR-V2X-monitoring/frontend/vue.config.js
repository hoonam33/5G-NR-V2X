var path = require("path")
const { defineConfig } = require('@vue/cli-service')
const { DefinePlugin } = require('webpack');

module.exports = defineConfig({
  transpileDependencies: true,

  configureWebpack: {
    devtool: process.env.NODE_ENV === 'production'
      ? 'hidden-source-map'
      : 'source-map',
    //mode: 'production',
    module: {
      rules: [
        {
          test: /\.geojson$/,
          loader: 'json-loader'
        },
        {
          test: /\.kml$/,
          loader: 'json-loader'
        },
        {
          test: /@?(BaseChart).*\.(ts|js)x?$/,
          loader: 'babel-loader'
        },
      ],
    },
    plugins: [
      new DefinePlugin({
        __VUE_PROD_HYDRATION_MISMATCH_DETAILS__: false, // 기능 플래그 정의
      }),
    ]
  },

  publicPath: '',
  outputDir: path.resolve(__dirname, "../backend/public/"),

  css: {
    loaderOptions: {
      scss: {
        additionalData: `@import "~@/assets/scss/variables.scss";`
      }
    }
  },

  pluginOptions: {
    vuetify: {
			// https://github.com/vuetifyjs/vuetify-loader/tree/next/packages/vuetify-loader
		}
  }
})
