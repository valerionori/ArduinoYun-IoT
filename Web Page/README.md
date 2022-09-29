# Web page 

Within this system, Arduino holds a web page, where the user can:

• Monitor the current silos level;

• Assign each silos to a specific deposit;

• Dynamically change the threshold of the minimum silos level that activates the system;

• Reset the above minimum threshold;

• Access the whole measurements history;

• Reset the above measurements history;

• Enable or Disable the automatic e-mail notification system;

• Notify the laboratory whenever any other problem occurs, specifying what it is.


### Silos Level Measurements

The system continuously measures silos’ level and holds
a copy of these measurements (along with date and time at which they were taken) on a
file called level.txt. On the web page, silos’ level is represented through a variation
of a meter, which changes color according to the silos current status and automatically
updates every 10 seconds.

### E-mail notification

The e-mail notification system is a further security measure. It
could happen that filling a silos takes too long, maybe because the operator forgets it, or
because some kind of problem occurs. If one silos becomes too empty, the technological
laboratory is noticed with an email that warns it about this critical condition. An empty
silos is dangerous since it could interrupt the correct reproduction of the ceramic slip
formula. This feature can be enabled or disabled from the web page.

### Silos/Deposit Configuration 

The operator can choose (usually, after technological
laboratory’s directives) which material a silos must be filled with, i.e., which deposit
assign to every silos. This operation can be done through specific buttons which explicit
the desired setup. Since this is a critical operation, the operator is warned
through a pop-up and invited to confirm the new setup. It is also possible to fill two
different silos with the same material (from one specific deposit). In this case, the Yellow
LED on the deposit will not turn off until both silos are filled.

### Silos Minimum Level Trigger

The minimum level of a silos can be freely modified
through the cursor on the web page. In fact, it could be that one silos
is becoming empty faster than another one, or faster than before, because of a change
in the requested percentage of that specific material. Thus, the system should activate
earlier, in order to avoid that it becomes empty. This threshold is specified in a file called
limit.txt. This value can be reset through a specific button.

### History Measurements

The operator can access the whole measurements history
(stored in the level.txt file) and, occasionally, reset it through a specific button.

### Secure Login 

The Web Panel access is secured by a user/password authentication
method in order to avoid unauthorized access to the system.


# HTML/JavaScript
### function SetLimit
This function is used to dynamically set the threshold of the minimum level sensor at which the system will detect it as nearly empt. Look at the arduino sketch for
more info.

### function SilosLevel
This function will dynamically display the current silos level on the meter.
Values are updated every 10 seconds in order to give arduino server enough time to process
the request/response. Look at the arduino sketch for more info
