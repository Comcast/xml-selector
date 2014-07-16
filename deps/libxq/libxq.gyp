{
  "targets": [
    {
      "target_name": "xq",
      "product_prefix": "lib",
      "type": "static_library",
      "include_dirs": [
        "./node",
        "<!(node -e \"var p = require('path'); console.log(p.join(p.dirname(require.resolve('libxmljs')),'vendor','libxml','include'))\")"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "./node",
          "<!(node -e \"var p = require('path'); console.log(p.join(p.dirname(require.resolve('libxmljs')),'vendor','libxml','include'))\")"
        ]
      },
      "sources": [
        "nodelist.c",
        "xq.c",
        "search.c",
        "traverse.c"
      ]
    }
  ]
}