import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
  id: window

  width: 700
  height: 700

  background: Image {
      anchors.centerIn: parent

      source: "image://svg/:/tiger.svg"
      sourceSize: {
        const min = Math.min(parent.width, parent.height)
        return Qt.size(min, min)
      }
    }

  color: "white"

  visible: true

  Row {
    anchors.centerIn: parent

    spacing: 5

    Button {
      icon.source: "image://svg/:/tiger.svg"

      text: "Ignore"

      onClicked: window.close()
    }

    Button {
      icon.source: "image://svg/:/tick.svg"

      text: "OK"

      onClicked: window.close()
    }

    Button {
      icon.source: "image://svg/:/cross.svg"

      text: "Cancel"

      onClicked: window.close()
    }
  }

  Image {
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    source: "image://svg/:/velocimetro3.svg"

    sourceSize: Qt.size(150, 150)
  }
}
