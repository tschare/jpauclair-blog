package  
{
	import com.adobe.serialization.json.JSON;
	import com.adobe.serialization.json.SimpleClass;
	import flash.display.Sprite;
	import flash.events.TimerEvent;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;
	import flash.utils.Timer;
	/**
	 * ...
	 * @author 
	 */
	public class Main extends Sprite
	{
		
		public function Main() 
		{
			
			var tim:Timer = new Timer(5000, 1);
			tim.addEventListener(TimerEvent.TIMER, OnTimer);
			tim.start();
		}
		
		private function OnTimer(e:TimerEvent):void 
		{
var tx:TextField = new TextField();
			addChild(tx);
			tx.width = 800;

			var result:String = null;
			var myAmf:Amf3Input = null;
			
			//var isTrue:Boolean = true;
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(isTrue);
			//tx.appendText("Boolean true result: " + result + "\n");
			//
			//var isFalse:Boolean = false;
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(isFalse);
			//tx.appendText("Boolean false result: " + result + "\n");			
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(1);
			//tx.appendText("int 1 result: " + result + "\n");			
//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(-1);
			//tx.appendText("int -1 result: " + result + "\n");						
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(int.MIN_VALUE);
			//tx.appendText("int int.MIN_VALUE result: " + result + "\n");						
//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(int.MAX_VALUE);
			//tx.appendText("int int.MAX_VALUE result: " + result + "\n");									
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(uint.MIN_VALUE);
			//tx.appendText("int uint.MIN_VALUE result: " + result + "\n");						
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(uint.MAX_VALUE);
			//tx.appendText("int uint.MAX_VALUE result: " + result + "\n");						
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(1.0343749837);
			//tx.appendText("float 1.0343749837 result: " + result + "\n");						
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(-38471.0343749837);
			//tx.appendText("float -38471.0343749837 result: " + result + "\n");						
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject("foobar");
			//tx.appendText("String foobar result: " + result + "\n");									
			//
			//var assoArrayTest:Array = new Array();
			//assoArrayTest.push(22);
			//assoArrayTest["foo"] = "bar";
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(["foobar",-1,1,32.34,["test"],assoArrayTest]);
			//tx.appendText("Array [\"foobar\"] result: " + result + "\n");												
			//
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(new SimpleClass2());
			//tx.appendText("SimpleClass result: \n" + result + "\n");			
			//
			//tx.appendText(JSON.encode(new SimpleClass2()));
			
			//var foo:String = "foobar";
			//myAmf = new Amf3Input();
			//result = myAmf.readObject(foo);
			//tx.appendText("String foobar result: " + result + "\n");						
			
			//var ba2:ByteArray = new ByteArray();
			//var tc:testClass = new testClass();
			//tc.size1 = -2;
			//tc.size2 = 1;
			//ba2.position = 0;
			//ba2.writeObject(tc.size1);
			//ba2.position = 0;
			//var output:String = "";
			//while (ba2.bytesAvailable > 0)
			//{
				//output += ba2.readUnsignedByte() + ","
			//}
			//tx.appendText("foo " +output + "\n");
			//
			//tc.size1 = 1;
			//tc.size2 = 1;
			//ba2.position = 0;
			//ba2.writeObject(tc.size2);
			//ba2.position = 0;
			//output = "";
			//while (ba2.bytesAvailable > 0)
			//{
				//output += ba2.readUnsignedByte() + ","
			//}
			//tx.appendText("foo " +output);			
			//
			//ba2.position = 0;
			//ba2.writeObject(int(-1));
			//ba2.position = 0;
			//var obj:* = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nSimpleRead -1: " +output);			
			//
			//ba2.position = 0;
			//ba2.writeObject(int(1));
			//ba2.position = 0;
			//var obj:* = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nSimpleRead 1: " +output);						
			//
			//ba2.position = 0;
			//ba2.writeObject(uint(1));
			//ba2.position = 0;
			//var obj:* = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nSimpleRead ui 1: " +output);									
			//
			//ba2.position = 0;
			//ba2.writeObject(uint(-1));
			//ba2.position = 0;
			//var obj:* = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nSimpleRead ui -1: " +output);												
			//
			//ba2.position = 0;
			//ba2.writeObject(int.MIN_VALUE);
			//ba2.position = 0;
			//obj = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nint.MIN_VALUE: " +output);															
			//
			//ba2.position = 0;
			//ba2.writeObject(int.MAX_VALUE);
			//ba2.position = 0;
			//obj = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nint.MAX_VALUE: " +output);															
			//
			//ba2.position = 0;
			//ba2.writeObject(uint.MIN_VALUE);
			//ba2.position = 0;
			//obj = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nuint.MIN_VALUE: " +output);																		
			//
			//ba2.length = 0;
			//ba2.position = 0;
			//ba2.writeObject(-(int.MAX_VALUE>>3)-1);
			//ba2.position = 0;
			//obj = ba2.readObject();
			//output = obj.toString();
			//tx.appendText("\nuint.MAX_VALUE: " +output);																		
			//ba2.position = 0;
			//output = "";
			//while (ba2.bytesAvailable > 0)
			//{
				//output += ba2.readUnsignedByte() + ","
			//}			
			//tx.appendText("foo " +output);			
			
			
			tx.height = 900;
			//return;
			var i:int = 0;
			var t:int = getTimer();
			var s:String = null;
			//for (i = 0; i < 4000; i++)
			//{
			//
				//s = JSON.encode(new SimpleClass2());
			//}
			//t = getTimer() - t;
			//tx.text = t.toString() + ", " + s;

			t = getTimer();
			var amf:Amf3Input = new Amf3Input();
			for (i = 0; i < 4000; i++)
			{
				s = amf.readObject(new SimpleClass2());
				//s = Amf3Input.JSON.encodeBA(new SimpleClass());
			}
			t = getTimer() - t;
			
			tx.appendText("\n" +   t.toString() + ", " + s);			
		}
		
	}

}