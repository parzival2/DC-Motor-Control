from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo
from PyQt5.QtCore import QObject, QIODevice, Qt
from PyQt5.QtWidgets import QApplication, QWidget
import pyqtgraph as pg
from Scripts.RPMPlotter_Ui import Ui_Form
import re
pg.setConfigOption('background', 'w')
import csv

class RPMPlotter(QWidget, Ui_Form):
    def __init__(self):
        super(RPMPlotter, self).__init__()
        # Setupui
        self.setupUi(self)
        # Create a serial port
        self.__serialPort = QSerialPort()
        # Setup Serialport
        # We are using USB serial driver, so the baud rate doesn't matter
        self.__serialPort.setBaudRate(QSerialPort.Baud115200, QSerialPort.Input)
        # Get the list of all available serial ports
        portsList = QSerialPortInfo.availablePorts()
        # Only connect to Chibios port
        chibiOsPort = None
        self.setWindowTitle("Experiment Data Collector")
        for port in portsList:
            if ("ChibiOS" in port.description()):
                chibiOsPort = port
                print(chibiOsPort.description())
        # Check of the device is connected.
        if (chibiOsPort is None):
            # We didn't find anything
            statusString = "Cannot find Chibios based device."
            print(statusString)
        else:
            # Set the serial port
            self.__serialPort.setPort(chibiOsPort)
            self.__serialPort.setDataBits(QSerialPort.Data8)
            self.__serialPort.setFlowControl(QSerialPort.NoFlowControl)
            self.__serialPort.setParity(QSerialPort.NoParity)
            self.__serialPort.setStopBits(QSerialPort.OneStop)
            # Connect signals and slots
            self.__serialPort.readyRead.connect(self.__onSerialPortReadyRead)
            self.startButton.clicked.connect(self.onStartButtonClicked)
            self.lineEdit.returnPressed.connect(self.onReferenceValueChanged)
            # Open the device
            self.__serialPort.open(QIODevice.ReadWrite)
            # Initialize variables
            # We track the reference and draw it as a line.
            self.__referenceArray = []
            # Timestamp in seconds
            self.__timeStampArray = []
            # RPM value
            self.__rpmArray = []
            # Voltage provided to the motor
            self.__voltageValue = []
            ## Curves
            self.__rpmCurve = pg.PlotCurveItem()
            self.__rpmCurve.setPen(pg.mkPen(color=(3, 39, 28), width=1.2))
            self.__referenceCurve = pg.PlotCurveItem()
            self.__referenceCurve.setPen(pg.mkPen(color=(214, 118, 17), width=1.2, style=Qt.DashDotLine))
            self.graphicsView.addItem(self.__rpmCurve)
            self.graphicsView.addItem(self.__referenceCurve)

    def onStartButtonClicked(self):
        self.__rpmArray.clear()
        self.__referenceArray.clear()
        self.__timeStampArray.clear()
        self.__rpmCurve.clear()
        message = "c\r\n"
        self.__serialPort.write(message.encode())

    def onReferenceValueChanged(self):
        value = int(self.lineEdit.text())
        print("Reference value : ", value)
        message = "s"+str(value)+"\r\n"
        self.__serialPort.write(message.encode())

    def __onSerialPortReadyRead(self):
        numbersRegex = re.compile(r"[-+]?\d*\.?\d+")
        while (self.__serialPort.canReadLine()):
            bytesRead = self.__serialPort.readLine()
            strBytesRead = str(bytesRead)
            floatsfound = numbersRegex.findall(strBytesRead)
            if (floatsfound):
                self.__timeStampArray.append(float(floatsfound[0]))
                self.__rpmArray.append(float(floatsfound[1]))
                self.__referenceArray.append(float(floatsfound[3]))
                self.__rpmCurve.setData(self.__timeStampArray, self.__rpmArray)
                self.__referenceCurve.setData(self.__timeStampArray, self.__referenceArray)

    def close(self):
        print("Closing")

if __name__ == '__main__':
    app = QApplication([])
    experiment = RPMPlotter()
    experiment.show()
    app.exec_()

