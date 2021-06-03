# Security_access_with_wifi

* Check if the RFID a user has has authorized access to operate the door.
* If yes...
  -Ask the user to input either of the 3 commands:
    O for Open Door.
    C for Close Door.
    E for logout.
   -Door is opened or closed based on the input and the previous state of the door.
* If no...
  -Ignore this attempt to access the door.

*Based on the login and logout status of an user that has authorized access to the door, an api url is genrated.
Based on the status, the data is updated in database*
