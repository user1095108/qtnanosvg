![screenshot.png](screenshot.png?raw=true)
# qtnanosvg
These are some utilities ported over from the [CairoWidget](https://github.com/user1095108/cairowidget) project. Qt's own SVG rendering is notoriously slow and buggy, it can even crash your program. [NanoSVG](https://github.com/memononen/nanosvg), while also buggy and incomplete, is an easy and reliable alternative. Using vector graphics in your project solves the problem of resolution-independent resources for your buttons and other controls.

If [SVG](https://en.wikipedia.org/wiki/SVG) implementations of [NanoSVG](https://github.com/memononen/nanosvg) or [Qt](https://en.wikipedia.org/wiki/Qt_(software)) were complete, implementing animated gauges and other rich controls would become easier.
# build instructions
    git submodule update --init
    qmake
    make
