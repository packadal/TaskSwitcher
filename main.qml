import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import windowing 1.0;

ApplicationWindow {
  id: mainWindow
  visible: true
  width: 0;
  height: 0;
  flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

  Component.onCompleted: {
    width = Screen.desktopAvailableWidth
    height = Screen.height - 1 //without this '-1' the app does not hide properly
    x = 0
    y = 0
  }

  Controller {
    id: ctrl

    onGuideChanged: {
      if(guide) {
        if(!mainWindow.visible)
          windowList.focusAndUpdate()
        mainWindow.visible = !mainWindow.visible;
      }
    }

    onRtChanged: {
      if(rt > 50)
        rightTriggerMenu.enabled = true
      else
        rightTriggerMenu.enabled = false
    }

    onLtChanged: {
      if(lt > 50)
        leftTriggerMenu.enabled = true
      else
        leftTriggerMenu.enabled = false
    }

    onAChanged: {
      if(!mainWindow.visible)
        return;

      if(a) {
        if(leftTriggerMenu.enabled) {
          leftTriggerMenu.activate();
          return;
        }

        if(rightTriggerMenu.enabled) {
          rightTriggerMenu.activate();
          return;
        }

        var activeWindow = test.model[test.currentIndex];
        if(activeWindow.isRunning)
          activeWindow.setFocus()
        else
          activeWindow.start()
        mainWindow.hide();
      }
    }

    onBChanged: {
      if(!mainWindow.visible)
        return;

      if(leftTriggerMenu.enabled || rightTriggerMenu.enabled) {
        return;
      }

      var activeWindow = test.model[test.currentIndex];
      if(activeWindow.isRunning)
        activeWindow.close()
    }
  }

  DesktopActions {
    id: desktop
  }

  Item {
    id: background
    anchors.fill: parent
    visible: false

    Image {
      source: "/images/kde_air-wide.jpg"
      anchors.fill: parent

      Item {
        id: clock
        anchors.top: parent.top
        anchors.left: parent.left
        width: 200
        height: 200

        property int hours
        property int minutes
        property int seconds
        property real shift
        property bool night: false

        function timeChanged()  {
          var date = new Date;
          hours = shift ? date.getUTCHours() + Math.floor(clock.shift) : date.getHours()
          night = ( hours < 7 || hours > 19 )
          minutes = shift ? date.getUTCMinutes() + ((clock.shift % 1) * 60) : date.getMinutes()
          seconds = date.getUTCSeconds();
        }

        Timer  {
          interval: 500; running: true; repeat: true;
          onTriggered: clock.timeChanged()
        }

        Text {
          id: hoursDisplay
          anchors.verticalCenter: parent.verticalCenter
          anchors.left: parent.left
          anchors.leftMargin: 16
          text: (clock.hours < 10 ? "0" : "") + clock.hours
          font.pixelSize: 80
          color: "white"
        }
        Text {
          id: separator
          anchors.verticalCenter: parent.verticalCenter
          anchors.left: hoursDisplay.right
          anchors.leftMargin: 16
          text: ":"
          font.pixelSize: hoursDisplay.font.pixelSize
          font.family: hoursDisplay.font.family
          color: hoursDisplay.color
        }
        Text {
          id: minutesDisplay
          anchors.verticalCenter: parent.verticalCenter
          anchors.left: separator.right
          anchors.leftMargin: 16
          text: (clock.minutes < 10 ? "0" : "") + clock.minutes
          font.pixelSize: hoursDisplay.font.pixelSize
          font.family: hoursDisplay.font.family
          color: hoursDisplay.color
        }
      }

      GridView {
        id: test

        property bool enabled: !rightTriggerMenu.enabled && !leftTriggerMenu.enabled
        property int angle: ctrl.leftStickAngle
        onAngleChanged: {
          if(!enabled || angle < 0)
            return;

          //up
          if(angle > 30 && angle < 150)
          {
            test.currentIndex -= 3
            if(test.currentIndex < 0)
              test.currentIndex += 9
          }

          //down
          if(angle > 210 && angle < 330)
          {
            test.currentIndex += 3
            if(test.currentIndex > 8)
              test.currentIndex -= 9
          }

          //left
          if(angle > 120 && angle < 240)
          {
            if(test.currentIndex % 3 == 0)
            {
              test.currentIndex += 3
            }
            test.currentIndex -= 1
          }

          //right
          if(angle > 300 || angle < 60)
          {
            test.currentIndex += 1
            if(test.currentIndex % 3 == 0)
            {
              test.currentIndex -= 3
            }
          }
        }

        width: parent.width *3/4
        height: parent.height *3/4
        anchors.centerIn: parent
        anchors.verticalCenterOffset: parent.height * 1 /16
        cellWidth: width/3
        cellHeight: height/3
        model: windowList.windows()
        currentIndex: 4

        property int previousIndexIndex: -1
        property variant previousIndexes: [-1, -1]
        onCurrentIndexChanged: {
          previousIndexIndex = (previousIndexIndex + 1) % 2

          var newArray = previousIndexes
          newArray[previousIndexIndex] = currentIndex
          previousIndexes = newArray
        }

        delegate: Item {
          id: container
          width: test.cellWidth
          height: test.cellHeight

          property bool focused: test.currentIndex === index

          Rectangle {
            id: outline
            anchors.centerIn: parent
            width: container.width - (container.focused ? 0 : 50)
            height: container.height - (container.focused ? 0 : 50)
            Behavior on width { NumberAnimation {} }
            Behavior on height { NumberAnimation {} }
            border.width: 1
            border.color: "black"
            Behavior on color { ColorAnimation {} }
            color: "lightgray"
            visible: false
          }
          Glow {
            opacity: container.focused ? 0.8 : 0.5
            Behavior on opacity { NumberAnimation {} }
            anchors.fill: outline
            source: outline
            radius: 16//container.focused ? 16 : 0
            samples: 16//container.focused ? 16 : 0
//            transparentBorder: true
          }

          Thumbnail {
            window: modelData
            width: outline.width - 100
            height: outline.height - 50
            anchors.centerIn: parent

            Image {
              id: windowIcon
              anchors.horizontalCenter: parent.horizontalCenter
              anchors.verticalCenter: parent.verticalCenter
              anchors.verticalCenterOffset: 0
              source: modelData.iconPath
              width: outline.height - 50
              height: outline.height - 50
              visible: false
            }
            Glow {
              anchors.fill: windowIcon
              source: windowIcon
              radius: container.focused ? 16 : 0
              samples: container.focused ? 16 : 0
              transparentBorder: true
              visible: !modelData.isRunning
            }

            Item {
              anchors.top: parent.bottom
              anchors.horizontalCenter: parent.horizontalCenter
              width: title.width + windowRunningIcon.width + 16
              height: Math.max(title.height, windowRunningIcon.height)

              Image {
                id: windowRunningIcon
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source: modelData.iconPath
                width: 30
                height: 30
                visible: false
              }
              Glow {
                anchors.fill: windowRunningIcon
                source: windowRunningIcon
                radius: container.focused ? 16 : 0
                samples: container.focused ? 16 : 0
                transparentBorder: true
                visible: modelData.isRunning
              }
              Text {
                id: title
                anchors.left: modelData.isRunning ? windowRunningIcon.right : undefined
                anchors.horizontalCenter: modelData.isRunning ? undefined : parent.horizontalCenter
                anchors.leftMargin: 16
                anchors.verticalCenter: windowRunningIcon.verticalCenter
                text: modelData.title
                font.pixelSize: 15
                color: "black"
              }
            }


          }
        }
      }
    }
  }
  GaussianBlur {
    anchors.fill: background
    source: background
    radius: (rightTriggerMenu.enabled || leftTriggerMenu.enabled) ? 16 : 0
    Behavior on radius { NumberAnimation {} }
    samples: 16
  }

  CircularMenu {
    id: rightTriggerMenu
    Item {
      id: mod
      Item {
        property string title: "Shutdown"
        property string iconPath: "/images/system-shutdown.png"
        function activate() {
          desktop.shutdown()
        }
        property bool isRunning: false
      }
      Item {
        property string title: "Reboot"
        property string iconPath: "/images/system-reboot.png"
        function activate() {
          desktop.restart()
        }
        property bool isRunning: false
      }
      Item {
        property string title: "Exit"
        property string iconPath: "/images/exit.png"
        function activate() {
          mainWindow.close()
        }
        property bool isRunning: false
      }
    }

    model: mod.children
    angle: ctrl.leftStickAngle
    anchors.horizontalCenterOffset: 400

    function activate()
    {
      rightTriggerMenu.model[activeIndex].activate()
      mainWindow.hide();
    }
  }

  CircularMenu {
    id: leftTriggerMenu
    model:  windowList.pinnedWindows()
    angle: ctrl.leftStickAngle
    anchors.horizontalCenterOffset: -400

    function activate()
    {
      var activeWindow = leftTriggerMenu.model[activeIndex];
      if(activeWindow.isRunning)
        activeWindow.setFocus()
      else
        activeWindow.start()
      mainWindow.hide();
    }
  }
}
