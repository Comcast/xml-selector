{
  "targets": [
    {
      "target_name": "xqjs",
      "include_dirs": [
        "<!(node -e \"console.log(require('path').dirname(require.resolve('libxmljs')))\")",
        "<!(node -e \"var p = require('path'); console.log(p.join(p.dirname(require.resolve('libxmljs')),'src'))\")",
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "ext/xqjs.cpp",
        "ext/xQWrapper.cpp"
      ],
      "dependencies": [
        "deps/libxq/libxq.gyp:xq"
      ]
    }
  ]
}
