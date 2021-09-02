![screenshot.png](screenshot.png?raw=true)
# qtnanosvg
These are some utilities ported over from the [CairoWidget](https://github.com/user1095108/cairowidget) project. Qt's own SVG rendering is notoriously slow and buggy, it can even crash your program. [NanoSVG](https://github.com/memononen/nanosvg), while also buggy and incomplete, is an easy and reliable alternative. Using vector graphics in your project solves the problem of resolution-independent resources for your buttons and other controls.

If [NanoSVG](https://github.com/memononen/nanosvg) or Qt's SVG implementations were more complete, it would be possible to implement animated gauges and other rich controls, with ease.
# build instructions
    git submodule update --init
    qmake
    make
