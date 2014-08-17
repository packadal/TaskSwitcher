import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
  id: triggerMenu
  width: 800
  height: width
  anchors.centerIn: parent

  property variant model
  property bool enabled: false
  state: enabled ? "visible" : "hidden"

  property int activeIndex: 0

  property int angle: -1

  states: [
    State {
      name: "visible"
      PropertyChanges {
        target: triggerMenu
        opacity: 1.0
      }
    },
    State {
      name: "hidden"
      PropertyChanges {
        target: triggerMenu
        opacity: 0.0
      }
    }
  ]
  Behavior on opacity { NumberAnimation { duration: 250; easing.type: Easing.InQuad } }


  RadialGradient {
    id: background
    anchors.fill: parent
    gradient: Gradient {
      GradientStop { position: 0.0; color: "lightgray" }
      GradientStop { position: 0.3; color: "lightgray" }
      GradientStop { position: 0.5; color: "transparent" }
    }
  }

  ConicalGradient {
    id: focusIndicator
    anchors.fill: parent
    angle: -triggerMenu.angle + 90
    property bool active: triggerMenu.angle >= 0
    gradient: Gradient {
      GradientStop { position: 0.0; color: focusIndicator.active ? Qt.lighter("lightgray", 0.2) : "transparent"
        Behavior on color { ColorAnimation {} } }
      GradientStop { position: 0.1; color: "transparent" }
      GradientStop { position: 0.9; color: "transparent" }
      GradientStop { position: 1.0; color: focusIndicator.active ? Qt.lighter("lightgray", 0.2) : "transparent"
        Behavior on color { ColorAnimation {} } }
    }


    source: background
//    visible: triggerMenu.angle >= 0
  }

  Repeater {
    model: triggerMenu.model
    delegate: Item {
      id: pinnedContainer
      anchors.centerIn: triggerMenu
      anchors.horizontalCenterOffset: triggerMenu.width/3
      transform: Rotation {
        id: mainRotation
        origin.x: -pinnedContainer.anchors.horizontalCenterOffset
        angle: -index* 360/triggerMenu.model.length
      }

      Item {
        id: pinnedItem
        anchors.fill: parent
        rotation: -mainRotation.angle

        function isAngleInSector(angle, sectorStart, sectorEnd)
        {
          if(sectorEnd > sectorStart)
            return angle >= sectorStart && angle < sectorEnd
          else
            return angle >= 0 && (angle >= sectorStart || angle < sectorEnd)
        }

        property bool focused: isAngleInSector(triggerMenu.angle,
                                               (rotation - 360/triggerMenu.model.length/2 + 360) % 360,
                                               rotation + 360/triggerMenu.model.length/2)
        onFocusedChanged: if(focused) triggerMenu.activeIndex = index

        scale: focused ? 2.0 : 1.0
        Behavior on scale { NumberAnimation {} }

        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: pinnedIcon.bottom
          text: modelData.title
          font.pixelSize: 20
          color: "black"
        }

        RadialGradient {
          id: runningHighlight
          anchors.fill: pinnedIcon
          gradient: Gradient {
            GradientStop { position: 0.0; color: "gray" }
            GradientStop { position: 0.2; color: "white" }
            GradientStop { position: 0.5; color: "transparent" }
          }
          visible: modelData.isRunning

          SequentialAnimation {
            id: highlightAnimation
            running: true
            loops: Animation.Infinite

            property int duration: 800

            NumberAnimation { target: runningHighlight; property: "scale"; duration: highlightAnimation.duration; easing.type: Easing.InOutQuad; from: 1; to: 2 }
            NumberAnimation { target: runningHighlight; property: "scale"; duration: highlightAnimation.duration; easing.type: Easing.InOutQuad; from: 2; to: 1 }
          }
        }
        Image {
          id: pinnedIcon
          anchors.centerIn: parent
          source: modelData.iconPath
          width: 100
          height: 100
          smooth: true
        }
      }
    }
  }
}
