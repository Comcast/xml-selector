{
  "targets": [
    {
      "target_name": "xqjs",
      "include_dirs": [
        "<!(node -e \"console.log(require('path').dirname(require.resolve('libxmljs')))\")"
      ],
      "sources": [
        "ext/xqjs.cpp"
      ],
      "dependencies": [
        "deps/libxq/libxq.gyp:xq"
      ]
    }
  ]
}
