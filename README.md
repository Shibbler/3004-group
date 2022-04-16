# 3004-group


## Team number and member names
- Team 1
- Owen Allen
- Vivek Chand
- Lucas Colwell
- Max Dobrei

## Who did what in the project

We all worked in close collaboration, so much of these are just areas we led, with assistance from others. We all contributed equally in terms of hours worked (whether directly coding, discussing or assisting each other), and view this project as a result of equally shared work.

Owen Allen
    - Use Cases
    - Use Case Diagram
    - Traceability Matrix
    - Power On Off Mechanics
    - Press and Release timing
    - README.md

Vivek Chand
    - State Diagram
    - Battery status display when device is powered and during session
    - worked on button implementation for earlobe connectivity

Lucas Colwell
    - Traceability Matrix (with Owen)
    - UML Diagram
    - SoftOff mechanics
    - Intensity, Session Group and Session Type Select with button
    - CES Light blinking
    - drainBattery function and algorithm
    - earlobe and connection strength

Max Dobrei
    - Designed and implemented the Session class
    - Implemented the session history requirement through the saveSession, loadSession, saveSessions and loadSessions functions as well as creating the database file.
    - Implemented the loadSelections function
    - Completed the sequence diagrams for the project
## File organization of the deliverables.

The diagrams folder contains
- UML Diagram
    - finalProjectUMLWithSignalsToSlots.png
    - finalProjectUML.png
- Use Case Diagram
    - Use_Case_Diagram.png
- Use Case Descriptions
    Use_Cases.pdf


- Sequence Diagrams
    - 3004FinalSeq1-1B_LowBattery.drawio.png
    - 3004Final_Seq1-4A_UserStopsSession.drawio.png
    - 3004FinalSeq1-1C_CriticalBattery.drawio.png
    - 3004Final_Seq1-4B_SaveCurrentSession.drawio.png
    - 3004Final_Seq1-1D_Inactivity.drawio.png
    - 3004Final_Seq1-4C_ChangeIntensity.drawio.png
    - 3004Final_Seq1-1_AInsufficientBattery.drawio.png
    - 3004Final_Seq1-4D_EarclipsRemoved.drawio.png
    - 3004Final_Seq1-1_PoweringOnDevice.drawio.png
    - 3004Final_Seq1-4_SessionOccurs.drawio.png
    - 3004Final_Seq1-2A_LoadingSession.drawio.png
    - 3004Final_Seq1-3B_NoConnection.drawio.png
    - 3004Final_Seq1-3_ConnectionTest.drawio.png
    - 3004Final_Seq1-2B_SavingSession.drawio.png
    - 3004Final_Seq1-2_PreparingForSession.drawio.png
    - 3004Final_Seq1-3A_OkayConnection.drawio.png

- Traceability Matrix
    - finalProjectTraceabilityMatrix.pdf
- State Diagram
    - 3004Final_StateDiagram.drawio.png

The CES-Device folder contains all of the project files required to run the program in QT.
This includes:
- CES-Device.pro
- Session.cpp
- mainwindow.h
- CES-Device.pro.user
- Session.h
- mainwindow.ui
- CES-Device.pro.user.4d39118
- main.cpp
- CES-Device.pro.user.b063330
- mainwindow.cpp


## Tested scenarios: ones that work and ones that don’t
Tested scenarios are also described in the Traceability Matrix (finalProjectTraceabilityMatrix.pdf) which can be found in the diagrams folder.

This includes
- The Run Session use case
- Turning on the device
- Turning off the device at any point
- Saving a session
- Loading a session
- Creating and running a custom session

All these cases work, and we did not identify a test