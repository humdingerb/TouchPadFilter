# TouchPadFilter
Input_server filter that disables the TouchPad while typing.


Most OSs have drivers for laptops which will allow the automatic dismissal of mouse-down events while typing. Haiku is not, to my knowledge, one of them.
Fortunately for us, the API makes it remarkably simple to write filters to the input_server, which can listen in on the input stream and make any changes we deem 'useful'. TouchPadFilter takes care of this problem.

How to change sensitivity
The first time the filter is run, it will make a default settings file in ~/config/settings named "TouchPadFilter_settings". This is just a text file, containing a number representing in milliseconds the delay between when the last key-up message is handled and when the next mouse-down message will be allowed.

You can set this number to whatever you want, but I imagine anything more than 500 (half a second) would be counter-productive. Since the threshold is loaded on instantiation of the filter (eg, when the input_server is fired up) any changes you make won't be noticed until the server is restarted, manually (/system/servers/input_server -q), or on reboot.
