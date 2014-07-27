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
  //  x: Screen.width/2;
  //  y: Screen.height/2;
  //  Behavior on x { NumberAnimation {} }
  //  Behavior on y { NumberAnimation {} }
  //  Behavior on width { NumberAnimation {} }
  //  Behavior on height { NumberAnimation {} }
  flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

  Component.onCompleted: {
    width = Screen.desktopAvailableWidth
    height = Screen.height
    x = 0
    y = 0
  }

  Controller {
    id: ctrl

    onGuideChanged: {
      if(guide) {
        mainWindow.visible = !mainWindow.visible;
        if(mainWindow.visible)
          windowList.focusAndUpdate()
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

        var activeWindow = test.currentItem.children[1].window;
        if(activeWindow.isRunning())
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

      var activeWindow = test.currentItem.children[1].window;
      if(activeWindow.isRunning())
        activeWindow.close()
      mainWindow.hide();
    }
  }

  DesktopActions {
    id: desktop
  }

  Item {
    id: background
    anchors.fill: parent
//    visible: false

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
          text: clock.hours
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
          text: clock.minutes
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
        model: windowList
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

          property bool focused: test.currentIndex == index

          Rectangle {
            id: outline
            anchors.centerIn: parent
            width: container.width - (container.focused ? 0 : 50)
            height: container.height - (container.focused ? 0 : 50)
            Behavior on width { NumberAnimation {} }
            Behavior on height { NumberAnimation {} }
            color: (container.focused ? "gray" : "lightgray")
            Behavior on color { ColorAnimation {} }
            border.width: 1
            border.color: "black"
          }

          //        Item {
          //          id: selectionIndicator
          //          clip: true

          //          property int selectedIndex: test.currentIndex
          //          onSelectedIndexChanged: {
          //            if(index == selectedIndex)
          //            {
          //              width = container.width - 50
          //              height = container.height - 50
          //            }

          //            var direction;

          //            left.anchors.top = undefined
          //            left.anchors.bottom = selectionIndicator.bottom

          //            right.anchors.top = selectionIndicator.top
          //            right.anchors.bottom = undefined

          //            bottom.anchors.left = undefined
          //            bottom.anchors.right = selectionIndicator.right

          //            top.anchors.left = selectionIndicator.left
          //            top.anchors.right = undefined

          //            var previousIndex = test.previousIndexes[(test.previousIndexIndex + 1) % 2]

          //            if(index === selectedIndex) {
          //              right.height = 0;
          //              left.height = 0;
          //              top.width = 0;
          //              bottom.width = 0;

          //              appear.start()
          //            }
          //            else if(index === previousIndex) {
          //              right.height = selectionIndicator.height;
          //              left.height = selectionIndicator.height;
          //              top.width = selectionIndicator.width;
          //              bottom.width = selectionIndicator.width;

          //              disappear.start()
          //            }
          //            else {
          //              width = 0
          //              height = 0
          //            }
          //          }

          //          property int animationDuration: 100

          //          SequentialAnimation {
          //            id: disappear

          //            ParallelAnimation {
          //              NumberAnimation { target: top; property: "width"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: 0 }
          //              NumberAnimation { target: bottom; property: "width"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: 0 }
          //            }

          //            ParallelAnimation {
          //              NumberAnimation { target: left; property: "height"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: 0 }
          //              NumberAnimation { target: right; property: "height"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: 0 }
          //            }
          //          }

          //          SequentialAnimation {
          //            id: appear

          //            ParallelAnimation {
          //              NumberAnimation { target: top; property: "width"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: selectionIndicator.width }
          //              NumberAnimation { target: bottom; property: "width"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: selectionIndicator.width }
          //            }

          //            ParallelAnimation {
          //              NumberAnimation { target: left; property: "height"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: selectionIndicator.height }
          //              NumberAnimation { target: right; property: "height"; duration: selectionIndicator.animationDuration; easing.type: Easing.Linear; to: selectionIndicator.height }
          //            }
          //          }

          //          anchors.centerIn: parent
          //          width: 0
          //          height: 0

          //          property int thickness: 2

          //          Rectangle {
          //            id: top
          //            width: parent.width
          //            height: selectionIndicator.thickness
          //            anchors.top: parent.top
          //            anchors.left: parent.left
          //            color: "black"
          //          }

          //          Rectangle {
          //            id: bottom
          //            width: parent.width
          //            height: selectionIndicator.thickness
          //            anchors.bottom: parent.bottom
          //            anchors.left: parent.left
          //            color: "black"
          //          }

          //          Rectangle {
          //            id: left
          //            width: selectionIndicator.thickness
          //            height: parent.height
          //            anchors.left: parent.left
          //            anchors.bottom: parent.bottom
          //            color: "black"
          //          }

          //          Rectangle {
          //            id: right
          //            width: selectionIndicator.thickness
          //            height: parent.height
          //            anchors.right: parent.right
          //            anchors.bottom: parent.bottom
          //            color: "black"
          //          }
          //        }

          Thumbnail {
            window: handle
            width: outline.width - 100
            height: outline.height - 50
            anchors.centerIn: parent

            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              anchors.top: parent.bottom
              text: title
              font.pixelSize: 10
              color: "black"
            }
            Image {
              id: windowIcon
              anchors.horizontalCenter: parent.horizontalCenter
              anchors.verticalCenter: handle.isRunning() ? parent.bottom : parent.verticalCenter
              anchors.verticalCenterOffset: handle.isRunning() ? height : 0
              //            anchors.top: handle.isRunning() ? parent.bottom : parent.verticalCenter
              source: icon
              width: handle.isRunning() ? 30 : outline.height - 50
              height: handle.isRunning() ? 30 : outline.height - 50
            }
          }
        }
      }
    }
  }
//  GaussianBlur {
//    anchors.fill: background
//    source: background
//    radius: (rightTriggerMenu.enabled || leftTriggerMenu.enabled) ? 16 : 0
//    Behavior on radius { NumberAnimation {} }
//    samples: 16
//  }

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
        function isRunning() { return false; }
      }
      Item {
        property string title: "Reboot"
        property string iconPath: "/images/system-reboot.png"
        function activate() {
          desktop.restart()
        }
        function isRunning() { return false; }
      }
      Item {
        property string title: "Exit"
        property string iconPath: "/images/exit.png"
        function activate() {
          mainWindow.close()
        }
        function isRunning() { return false; }
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
    model:  pinnedApplications
    angle: ctrl.leftStickAngle
    anchors.horizontalCenterOffset: -400

    function activate()
    {
      var activeWindow = leftTriggerMenu.model[activeIndex];
      if(activeWindow.isRunning())
        activeWindow.setFocus()
      else
        activeWindow.start()
      mainWindow.hide();
    }
  }
}
