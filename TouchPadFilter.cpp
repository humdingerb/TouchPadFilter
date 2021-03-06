/* 
 Touchpad sensitivity filter
 Most windows/mac/etc laptops have a sensitivity filter/mechanism
 which protects the user from accidently rubbing the
 touchpad while typing and thus deleting text or moving
 the cursor to a new insertion point. I've been grousing
 about this to myself for months, and finally decided to
 write something for BeOS which other people might benefit 
 from.
 
 Just an input_server add_on. I wrote it in < hour. Works on
 my inspiron 3500, but I imagine it would work fine on any BeOS
 machine. Not that desktops would benefit. Compiled under 4.5.2
 
 Do what you want with it. Enjoy.
 
 --Shamyl Zakariya
 --March 22 2000
 --zakariya@earthlink.net
 
 Note:
 makes a settings file in home/config/settings, 
 touchpad_sensitivity_filter_prefs.txt Just a text file,
 containing the sensitivity threshold in thousandths of
 a second. This number represents the delay between the last
 B_KEY_UP message and when the filter will allow
 a B_MOUSE_DOWN message. Eg, if less than _threshold has 
 transpired between a B_KEY_DOWN and a B_MOUSE_DOWN, the mouse
 down event will be skipped, cast into oblivion.

 Kudos to the people at Be for making this kind of thing
 so damnably easy to do. Bravo!
 
 And a hearty thanks to Magnus Landahl for his superb
 debug console, without which I wouldn't have ever known if
 this damn thing worked at all.
 
*/


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Debug.h>
#include <List.h>
#include <Message.h>
#include <OS.h>
#include <StorageKit.h>

#include <add-ons/input_server/InputServerFilter.h>


extern "C" _EXPORT BInputServerFilter* instantiate_input_filter();

class TouchPadFilter : public BInputServerFilter 
{
	public:
		TouchPadFilter();
		virtual ~TouchPadFilter();
		virtual	filter_result Filter(BMessage *message, BList *outList);

	private:
		bigtime_t _lastKeyUp; //timestamp of last B_KEY_DOWN
		bigtime_t _threshold;
};


TouchPadFilter::TouchPadFilter()
{
	_lastKeyUp = 0;
	_threshold = 300; //~one third of a second?

	//load settings file

	BPath settingsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK)
	{
		settingsPath.Append("TouchPadFilter_settings");
		BEntry settingsEntry(settingsPath.Path());
		
		//does the settings file exist?
		if (!settingsEntry.Exists()) {
			BFile defaultSettingsFile(&settingsEntry, B_CREATE_FILE |
				B_READ_WRITE);

			if (defaultSettingsFile.IsWritable()) {
				//write in default settings
				char buf[64];
				sprintf(buf, "%ld #delay in thousandths", (int32)_threshold);
				defaultSettingsFile.WriteAt(0, (void *)buf, strlen(buf));
				defaultSettingsFile.Unset();
			}		
		} else {
			BFile settingsFile(&settingsEntry, B_READ_WRITE);
		
			if (settingsFile.IsReadable()) {
				char buf[256];
				uint32 amt_read = settingsFile.ReadAt(0, (void *)buf, 256);
				buf[amt_read] = '\0';
				_threshold = atoi(buf);
			}
		}
	}
	_threshold *= 1000; //I'd rather keep the number in the file in thousandths
}


TouchPadFilter::~TouchPadFilter()
{
}


filter_result TouchPadFilter::Filter(BMessage *message,
	BList *outList)
{
	filter_result res = B_DISPATCH_MESSAGE;
	
	switch (message->what)
	{
		case B_KEY_UP: case B_KEY_DOWN:
		{
			_lastKeyUp = system_time();	//update timestamp
			break;
		}
		
		case B_MOUSE_DOWN:
		{
			bigtime_t now = system_time();
			//if less than the threshold has passed, tell input server
			//to ignore this message
			if ((now - _lastKeyUp) < _threshold) res = B_SKIP_MESSAGE;
			break;
		}
	
		default:
		{
			//we don't want to mess with anybody else
			break;		
		}
	}	
	return (res);
}


//*************************************************************************
//Exported instantiator


BInputServerFilter* instantiate_input_filter()
{
	return (new TouchPadFilter());
}
