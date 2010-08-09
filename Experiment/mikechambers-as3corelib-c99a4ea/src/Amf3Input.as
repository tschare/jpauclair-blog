package
{
	import com.adobe.images.BitString;
	import flash.utils.ByteArray;
	import flash.utils.IExternalizable;

	public class Amf3Input
	{
		private static const COMMA:int = ",".charCodeAt(0);
		private static const COLON:int = ":".charCodeAt(0);
		
		private static const ARRAY_START:int = "[".charCodeAt(0);
		private static const ARRAY_END:int = "]".charCodeAt(0);

		private static const OBJECT_START:int = "{".charCodeAt(0);
		private static const OBJECT_END:int = "}".charCodeAt(0);
		
		private static const NULL_STR1:int = "n".charCodeAt(0);
		private static const NULL_STR2:int = "u".charCodeAt(0);
		private static const NULL_STR3:int = "l".charCodeAt(0);
		private static const NULL_STR4:int = "l".charCodeAt(0);
		
		private static const TRUE_STR1:int = "t".charCodeAt(0);
		private static const TRUE_STR2:int = "r".charCodeAt(0);
		private static const TRUE_STR3:int = "u".charCodeAt(0);
		private static const TRUE_STR4:int = "e".charCodeAt(0);
		
		private static const FALSE_STR1:int = "f".charCodeAt(0);
		private static const FALSE_STR2:int = "a".charCodeAt(0);
		private static const FALSE_STR3:int = "l".charCodeAt(0);
		private static const FALSE_STR4:int = "s".charCodeAt(0);		
		private static const FALSE_STR5:int = "e".charCodeAt(0);		
		
		
		private static const QUOTATION_MARK:int = "\"".charCodeAt(0);
		private static const QUOTATION_MARK_CONVERTED1:int = "\\".charCodeAt(0);
		private static const QUOTATION_MARK_CONVERTED2:int = "\"".charCodeAt(0) ;
		
		private static const SOLIDUS:int = "/".charCodeAt(0);
		private static const SOLIDUS_CONVERTED1:int = '\\'.charCodeAt(0);
		private static const SOLIDUS_CONVERTED2:int = "/".charCodeAt(0);
		
		private static const REVERSE_SOLIDUS:int = '\\'.charCodeAt(0);
		private static const REVERSE_SOLIDUS_CONVERTED1:int = "\\".charCodeAt(0)
		private static const REVERSE_SOLIDUS_CONVERTED2:int = "\\".charCodeAt(0)
		
		private static const BELL:int = '\b'.charCodeAt(0);
		private static const BELL_CONVERTED1:int = "\\".charCodeAt(0)
		private static const BELL_CONVERTED2:int = "b".charCodeAt(0)
		
		private static const FORM_FEED:int = '\f'.charCodeAt(0);
		private static const FORM_FEED_CONVERTED1:int = "\\".charCodeAt(0)
		private static const FORM_FEED_CONVERTED2:int = "f".charCodeAt(0)
						
		private static const NEW_LINE:int = '\n'.charCodeAt(0);
		private static const NEW_LINE_CONVERTED1:int = "\\".charCodeAt(0)
		private static const NEW_LINE_CONVERTED2:int = "n".charCodeAt(0)
						
		private static const CARRIAGE_RETURN:int = '\r'.charCodeAt(0);
		private static const CARRIAGE_RETURN_CONVERTED1:int = "\\".charCodeAt(0)
		private static const CARRIAGE_RETURN_CONVERTED2:int = "r".charCodeAt(0)
						
		private static const HORIZONTAL_TAB:int = '\t'.charCodeAt(0);
		private static const HORIZONTAL_TAB_CONVERTED1:int = "\\".charCodeAt(0)
		private static const HORIZONTAL_TAB_CONVERTED2:int = "t".charCodeAt(0)
	
	
		protected var objectTable:Array;
		protected var stringTable:Array;
		protected var traitsTable:Array;

		public var inAMF:ByteArray;
		public var outJSON:ByteArray;
		
		

		function Amf3Input()
		{			
			this.stringTable = new Array();
			this.objectTable = new Array();
			this.traitsTable = new Array();
			
			inAMF = new ByteArray();
			outJSON = new ByteArray();
		}

		public function reset():void
		{
			super.reset();
			this.stringTable.clear();
			this.objectTable.clear();
			this.traitsTable.clear();
		}

		public function  saveObjectTable():Object
		{
			var table:Object = this.objectTable;
			this.objectTable = new Array();
			return table;
		}

		public function restoreObjectTable(table:Object):void
		{
			this.objectTable = table as Array;
		}

		public function  saveTraitsTable():Object
		{
			var table:Object = this.traitsTable;
			this.traitsTable = new Array();
			return table;
		}

		public function  restoreTraitsTable(table:Object):void
		{
			this.traitsTable = table as Array;
		}

		public function  saveStringTable():Object
		{
			var table:Object = this.stringTable;
			this.stringTable = new Array();
			return table;
		}

		public function  restoreStringTable(table:Object):void
		{
			this.stringTable = table as Array;
		}

		
		public function  readObject(aObject:*):String
		{
			inAMF.position = 0;
			inAMF.writeObject(aObject);
			  
			inAMF.position = 0;
			outJSON.position = 0;
			
			InternalReadObject();

			outJSON.position = 0;
			var value:String = outJSON.readUTFBytes(outJSON.length);
			return value;
		}
		
		private function InternalReadObject() : void
		{
			var type:int = this.inAMF.readByte();
			readObjectValue(type);
		}

		protected function readObjectValue(type:int):void
		{
			var value:Object = null;

			switch (type)
			{
				case 6: // String
					outJSON.writeByte(QUOTATION_MARK);
					var ba:ByteArray = readString();
					ba.position = 0;
					outJSON.writeBytes(ba, 0, ba.length);
					outJSON.writeByte(QUOTATION_MARK);
					break;
				case 10: // Object
					outJSON.writeByte(OBJECT_START);
						readScriptObject();
					outJSON.writeByte(OBJECT_END);
					break;
				case 9:	// Array
					outJSON.writeByte(ARRAY_START);
						readArray();
					outJSON.writeByte(ARRAY_END);
					break;
				case 2:	// FALSE
					outJSON.writeByte(FALSE_STR1);
					outJSON.writeByte(FALSE_STR2);
					outJSON.writeByte(FALSE_STR3);
					outJSON.writeByte(FALSE_STR4);
					outJSON.writeByte(FALSE_STR5);
					break;
					
				case 3: // TRUE
					outJSON.writeByte(TRUE_STR1);
					outJSON.writeByte(TRUE_STR2);
					outJSON.writeByte(TRUE_STR3);
					outJSON.writeByte(TRUE_STR4);
					break;
				case 4: // Int / Uint Use native readUint29 parsing
				case 5: // Number Use native readUint29 parsing
					// Step back and read back as amf object.
					inAMF.position -= 1;
					var nb:String = inAMF.readObject().toString();
					outJSON.writeUTFBytes(nb);
					break;
				case 0: // Undefined
					DefaultAMFReadObject();
					//if (!this.isDebug) break label336;
					//this.trace.writeUndefined(); 
					break;
				case 1: // Null
					outJSON.writeByte(NULL_STR1);
					outJSON.writeByte(NULL_STR2);
					outJSON.writeByte(NULL_STR3);
					outJSON.writeByte(NULL_STR4);
				  break;
				case 7:	// Xml-Doc
					DefaultAMFReadObject();
					break;
				case 11: // Xml
					DefaultAMFReadObject();
					//value = readXml();
					break;
				case 8: // Date
					DefaultAMFReadObject();
					//Date
					//value = readDate();
					break;
				case 12: // ByteArray
					DefaultAMFReadObject();
					break;
				default:
					trace("Unknow type", type);
			}
		}
		

		private function DefaultAMFReadObject(stepBack:Boolean = true):*
		{
			if (stepBack) inAMF.position -= 1;
			return inAMF.readObject();
		}
		
		protected function readString():ByteArray
		{
			// Always start string with quotation mark (")
			var ref:int = readUInt29();

			if ((ref & 0x1) == 0)
			{
			  return getStringReference(ref >> 1);
			}

			var len:int = ref >> 1;

			if (0 == len)
			{
			  return null;
			}

			var baStr:ByteArray = new ByteArray();
			while (--len >= 0)
			{
				var charCode:uint = inAMF.readUnsignedByte();
				//Insert string escape character	
				switch ( charCode ) 
				{
				
					case QUOTATION_MARK:	// quotation mark
						baStr.writeByte(QUOTATION_MARK_CONVERTED1);
						baStr.writeByte(QUOTATION_MARK_CONVERTED2);
						break;
						
					//case '/':	// solidus
					//	s += "\\/";
					//	break;
						
					case REVERSE_SOLIDUS:	// reverse solidus
						baStr.writeByte(REVERSE_SOLIDUS_CONVERTED1);
						baStr.writeByte(REVERSE_SOLIDUS_CONVERTED2);
						break;
						
					case BELL:	// bell
						baStr.writeByte(BELL_CONVERTED1);
						baStr.writeByte(BELL_CONVERTED2);
						break;
						
					case FORM_FEED:	// form feed
						baStr.writeByte(FORM_FEED_CONVERTED1);
						baStr.writeByte(FORM_FEED_CONVERTED2);
						break;
						
					case NEW_LINE:	// newline
						baStr.writeByte(NEW_LINE_CONVERTED1);
						baStr.writeByte(NEW_LINE_CONVERTED2);
						break;
						
					case CARRIAGE_RETURN:	// carriage return
						baStr.writeByte(CARRIAGE_RETURN_CONVERTED1);
						baStr.writeByte(CARRIAGE_RETURN_CONVERTED2);
						break;
						
					case HORIZONTAL_TAB:	// horizontal tab
						baStr.writeByte(HORIZONTAL_TAB_CONVERTED1);
						baStr.writeByte(HORIZONTAL_TAB_CONVERTED2);
						break;
						
					default:	// everything else
						
						// check for a control character and escape as unicode
						//if ( ch < ' ' ) {
							// get the hex digit(s) of the character (either 1 or 2 digits)
							//var hexCode:String = ch.charCodeAt( 0 ).toString( 16 );
							//
							// ensure that there are 4 digits by adjusting
							// the # of zeros accordingly.
							//var zeroPad:String = hexCode.length == 2 ? "00" : "000";
							//
							// create the unicode escape sequence with 4 hex digits
							//s += "\\u" + zeroPad + hexCode;
						//} else {
						//
							baStr.writeByte(charCode);
							// no need to do any special encoding, just pass-through
							//s += ch;
							//
						//}
				}	// end switch
				
				
				
			}
			
			this.stringTable.push(baStr);

			return baStr;
		}

		//protected function  readDate():Date
		//{
			//var ref:int = readUInt29();
//
			//if ((ref & 0x1) == 0)
			//{
			  //return getObjectReference(ref >> 1) as Date;
			//}
//
			//var time:Number = this.inAMF.readDouble();
//
			//var d:Date = new Date(time);
//
			//this.objectTable.add(d);
			//
			//return d;
		//}

		protected function readArray():void
		{
			var ref:int = readUInt29();

			if ((ref & 0x1) == 0)
			{
			  getObjectReference(ref >> 1);
			}

			var len:int = ref >> 1;
			var array:Object = null;

			// Skip associative Array value
			var map:Object = null;
			while (true)
			{
				var name:ByteArray = readString();
				if (name == null) break; 
				if (name.length == 0) break;
				DefaultAMFReadObject(false);
			}

			for (var i:int = 0; i < len; ++i)
			{
				InternalReadObject();
				if (i < len - 1) outJSON.writeByte(COMMA);
			}
		}

		private function GetStringBytearrayAsClassicString(ba:ByteArray):String
		{
			ba.position = 0;
			return ba.readUTFBytes(ba.length);
		}
		
		protected function  readScriptObject():void
		{
			var ref:int = readUInt29();

			if ((ref & 0x1) == 0)
			{
			  getObjectReference(ref >> 1);
			}

			var ti:TraitsInfo = readTraits(ref);
			
			var len:int = ti.properties.length;
			for (var i:int = 0; i < len; ++i)
			{
				var prop:ByteArray = ti.properties[i];

				outJSON.writeByte(QUOTATION_MARK);
				outJSON.writeBytes(prop, 0, prop.length);
				outJSON.writeByte(QUOTATION_MARK);
				outJSON.writeByte(COLON);
				InternalReadObject();
				if (i<len-1) outJSON.writeByte(COMMA);
				//Object value = 
				//proxy.setValue(object, propName, value);
			}			
		
			if (ti.isDynamic)
			{
				var firstPass:Boolean = true;
				while (true)
				{
					var name:ByteArray = readString();
					if (name == null) break; 
					if (name.length == 0) break;
					if (firstPass)
					{
						firstPass = false;
					}
					else
					{
						outJSON.writeByte(COMMA);
					}
					outJSON.writeByte(QUOTATION_MARK);
						outJSON.writeBytes(name, 0, name.length);
					outJSON.writeByte(QUOTATION_MARK);					
					outJSON.writeByte(COLON);
					InternalReadObject();
					if (i<len-1) outJSON.writeByte(COMMA);
					
				}
			}
			  
			  
			//var className:String = ti.getClassName();
			//boolean externalizable = ti.isExternalizable();

			/*
			PropertyProxy proxy = null;

			var aliasedClass:String = ClassAliasRegistry.getRegistry().getClassName(className);
			if (aliasedClass != null)
			  className = aliasedClass;
			Object object;
			Object object;
			if ((className == null) || (className.length() == 0))
			{
			  object = new ASObject();
			}
			else if (className.startsWith(">"))
			{
			  Object object = new ASObject();
			  ((ASObject)object).setType(className);
			}
			else
			{
			  Object object;
			  if ((this.context.instantiateTypes) || (className.startsWith("flex.")))
			  {
				Class desiredClass = AbstractProxy.getClassFromClassName(className, this.context.createASObjectForMissingType);

				proxy = PropertyProxyRegistry.getRegistry().getProxyAndRegister(desiredClass);
				Object object;
				if (proxy == null)
				  object = ClassUtil.createDefaultInstance(desiredClass, null);
				else {
				  object = proxy.createInstance(className);
				}
			  }
			  else
			  {
				object = new ASObject();
				((ASObject)object).setType(className);
			  }
			}
			if (proxy == null) {
			  proxy = PropertyProxyRegistry.getProxyAndRegister(object);
			}

			int objectId = this.objectTable.size();
			this.objectTable.add(object);

			if (externalizable)
			{
			  readExternalizable(className, object);
			}
			else
			{
			  if (this.isDebug)
			  {
				this.trace.startAMFObject(className, this.objectTable.size() - 1);
			  }

			  int len = ti.getProperties().size();

			  for (int i = 0; i < len; ++i)
			  {
				String propName = ti.getProperty(i);

				if (this.isDebug) {
				  this.trace.namedElement(propName);
				}
				Object value = readObject();
				proxy.setValue(object, propName, value);
			  }

			  if (ti.isDynamic())
			  {
				while (true)
				{
				  String name = readString();
				  if (name == null) break; if (name.length() == 0)
					break;
				  if (this.isDebug) {
					this.trace.namedElement(name);
				  }
				  Object value = readObject();
				  proxy.setValue(object, name, value);
				}
			  }
			}

			if (this.isDebug) {
			  this.trace.endAMFObject();
			}

			Object newObj = proxy.instanceComplete(object);

			if (newObj != object)
			{
			  this.objectTable.set(objectId, newObj);
			  object = newObj;
			}
			
			outputBA.writeByte(OBJECT_END);
			*/
		}

		protected function  readExternalizable(className:String, object:Object):void
		{
			/*
			if (object is IExternalizable)
			{
			  ((Externalizable)object).readExternal(this);
			}
			else
			{
			  SerializationException ex = new SerializationException();
			  ex.setMessage(10305, new Object[] { object.getClass().getName() });
			  throw ex;
			}
			*/
		}

		protected function  readByteArray() : void
		{
			/*
			var ref:int = readUInt29();

			if ((ref & 0x1) == 0)
			{
			  return (byte[])(byte[])getObjectReference(ref >> 1);
			}

			int len = ref >> 1;

			byte[] ba = new byte[len];

			this.objectTable.add(ba);

			this.inAMF.readFully(ba, 0, len);

			if (this.isDebug) {
			  this.trace.startByteArray(this.objectTable.size() - 1, len);
			}
			return ba;
			*/
		}
			
			
		

		protected function  readTraits(ref:int):TraitsInfo
		{
			if ((ref & 0x3) == 1)
			{
				return getTraitReference(ref >> 2);
			}

			var ti:TraitsInfo = new TraitsInfo();
			ti.externalizable = (ref & 0x4) == 4;
			ti.isDynamic = (ref & 0x8) == 8;
			ti.properties = new Array();
			var count:int = ref >> 4;
			ti.className = readString();
			
			this.traitsTable.push(ti);

			for (var i:int = 0; i < count; ++i)
			{
			  ti.properties.push(readString());
			}
			
			

			return ti;
			
		}

		protected function  readUInt29():uint
		{
			var b:uint = inAMF.readUnsignedByte();

			if (b < 128)
			{
			  return b;
			}

			var value:uint = (b & 0x7F) << 7;
			b = inAMF.readUnsignedByte();

			if (b < 128)
			{
			  return value | b;
			}

			value = (value | b & 0x7F) << 7;
			b = inAMF.readUnsignedByte();

			if (b < 128)
			{
			  return value | b;
			}

			value = (value | b & 0x7F) << 8;
			b = inAMF.readUnsignedByte();

			return value | b;
		}

		protected function  getObjectReference(ref:int):Object
		{
			return this.objectTable[ref];
		}

		protected function  getStringReference(ref:int):ByteArray
		{
			return this.stringTable[ref];
		}

		protected function  getTraitReference(ref:int): TraitsInfo
		{
			return this.traitsTable[ref] as TraitsInfo;
		}
	}
}
import flash.utils.ByteArray;

internal class TraitsInfo
{
	public var className:ByteArray;
	public var isDynamic:Boolean;
	public var externalizable:Boolean;
	public var properties:Array;
}