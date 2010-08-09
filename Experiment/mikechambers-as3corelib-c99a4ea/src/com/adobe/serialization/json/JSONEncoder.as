/*
  Copyright (c) 2008, Adobe Systems Incorporated
  All rights reserved.

  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are
  met:

  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the 
    documentation and/or other materials provided with the distribution.
  
  * Neither the name of Adobe Systems Incorporated nor the names of its 
    contributors may be used to endorse or promote products derived from 
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package com.adobe.serialization.json 
{

	import flash.utils.ByteArray;
	import flash.utils.describeType;

	public class JSONEncoder {
		
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
		
		/** The string that is going to represent the object we're encoding */
		private var jsonString:String;
		private var outputBA:ByteArray;
		
		/**
		 * Creates a new JSONEncoder.
		 *
		 * @param o The object to encode as a JSON string
		 * @langversion ActionScript 3.0
		 * @playerversion Flash 9.0
		 * @tiptext
		 */
		private var mUseByteArray:Boolean = false;
		public function JSONEncoder( value:*, useBA:Boolean = false ) {
			outputBA = new ByteArray();
			mUseByteArray = useBA;
			jsonString = convertToString( value );
		
		}
		
		/**
		 * Gets the JSON string from the encoder.
		 *
		 * @return The JSON string representation of the object
		 * 		that was passed to the constructor
		 * @langversion ActionScript 3.0
		 * @playerversion Flash 9.0
		 * @tiptext
		 */
		public function getString():String {
			if (mUseByteArray)
			{
				outputBA.position = 0;
				return outputBA.readUTFBytes(outputBA.length);
				
			}
			else
			{
				return jsonString;
			}
			
		}
		/**
		 * Converts a value to it's JSON string equivalent.
		 *
		 * @param value The value to convert.  Could be any 
		 *		type (object, number, array, etc)
		 */
		private function convertToString( value:* ):String 
		{
		
			if (mUseByteArray)
			{
				// determine what value is and convert it based on it's type
				if ( value is String ) 
				{
					outputBA.writeUTFBytes(value);
					return null;
				} 
				else if ( value is Number ) 
				{
					if (isFinite( value as Number))
					{
						outputBA.writeUTFBytes(value.toString());
					}
					else
					{
						outputBA.writeByte(NULL_STR1);
						outputBA.writeByte(NULL_STR2);
						outputBA.writeByte(NULL_STR3);
						outputBA.writeByte(NULL_STR4);
					}
					return null;
				} 
				else if ( value is Boolean ) 
				{
					if (value)
					{
						outputBA.writeByte(TRUE_STR1);
						outputBA.writeByte(TRUE_STR2);
						outputBA.writeByte(TRUE_STR3);
						outputBA.writeByte(TRUE_STR4);
					}
					else
					{
						outputBA.writeByte(FALSE_STR1);
						outputBA.writeByte(FALSE_STR2);
						outputBA.writeByte(FALSE_STR3);
						outputBA.writeByte(FALSE_STR4);
						outputBA.writeByte(FALSE_STR5);
					}

					return null;

				} 
				else if ( value is Array ) {
				
					// call the helper method to convert an array
					return arrayToString( value as Array );
				
				} else if ( value is Object && value != null ) {
				
					// call the helper method to convert an object
					return objectToString( value );
				}
				
				outputBA.writeByte(NULL_STR1);
				outputBA.writeByte(NULL_STR2);
				outputBA.writeByte(NULL_STR3);
				outputBA.writeByte(NULL_STR4);
				
				return null;
				
			}
			else
			{
				// determine what value is and convert it based on it's type
				if ( value is String ) 
				{
					// escape the string so it's formatted correctly
					return escapeString( value as String );
					
				} 
				else if ( value is Number ) 
				{
					// only encode numbers that finate
					return isFinite( value as Number) ? value.toString() : "null";

				} else if ( value is Boolean ) {
					// convert boolean to string easily
					return value ? "true" : "false";

				} else if ( value is Array ) {
				
					// call the helper method to convert an array
					return arrayToString( value as Array );
				
				} else if ( value is Object && value != null ) {
				
					// call the helper method to convert an object
					return objectToString( value );
				}
				
				return "null";
			}
		}
		
		/**
		 * Escapes a string accoding to the JSON specification.
		 *
		 * @param str The string to be escaped
		 * @return The string with escaped special characters
		 * 		according to the JSON specification
		 */
		private var tempBA:ByteArray = new ByteArray();
		private function escapeString( str:String ):String 
		{
			if (mUseByteArray)
			{
				outputBA.writeByte(QUOTATION_MARK);
				
				tempBA.clear();
				tempBA.writeUTFBytes(str);
				
				var lenBA:int = tempBA.length;
				var i:int =0
				
				for ( i = 0; i < lenBA; i++ ) 
				{
					var currentCharCode:int = tempBA[i];
					switch ( currentCharCode ) 
					{
					
						case QUOTATION_MARK:	// quotation mark
							outputBA.writeByte(QUOTATION_MARK_CONVERTED1);
							outputBA.writeByte(QUOTATION_MARK_CONVERTED2);
							break;
							
						//case '/':	// solidus
						//	s += "\\/";
						//	break;
							
						case REVERSE_SOLIDUS:	// reverse solidus
							outputBA.writeByte(REVERSE_SOLIDUS_CONVERTED1);
							outputBA.writeByte(REVERSE_SOLIDUS_CONVERTED2);
							break;
							
						case BELL:	// bell
							outputBA.writeByte(BELL_CONVERTED1);
							outputBA.writeByte(BELL_CONVERTED2);
							break;
							
						case FORM_FEED:	// form feed
							outputBA.writeByte(FORM_FEED_CONVERTED1);
							outputBA.writeByte(FORM_FEED_CONVERTED2);
							break;
							
						case NEW_LINE:	// newline
							outputBA.writeByte(NEW_LINE_CONVERTED1);
							outputBA.writeByte(NEW_LINE_CONVERTED2);
							break;
							
						case CARRIAGE_RETURN:	// carriage return
							outputBA.writeByte(CARRIAGE_RETURN_CONVERTED1);
							outputBA.writeByte(CARRIAGE_RETURN_CONVERTED2);
							break;
							
						case HORIZONTAL_TAB:	// horizontal tab
							outputBA.writeByte(HORIZONTAL_TAB_CONVERTED1);
							outputBA.writeByte(HORIZONTAL_TAB_CONVERTED2);
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
								outputBA.writeByte(currentCharCode);
								// no need to do any special encoding, just pass-through
								//s += ch;
								//
							//}
					}	// end switch
					
				}	// end for loop
				outputBA.writeByte(QUOTATION_MARK);	
			}
			else
			{
				
				// create a string to store the string's jsonstring value
				var s:String = "";
				// current character in the string we're processing
				var ch:String;
				
				// store the length in a local variable to reduce lookups
				var len:int = str.length;

			
				// loop over all of the characters in the string
				for ( i = 0; i < len; i++ ) {
				
					// examine the character to determine if we have to escape it
					ch = str.charAt( i );
					switch ( ch ) {
					
						case '"':	// quotation mark
							s += "\\\"";
							break;
							
						//case '/':	// solidus
						//	s += "\\/";
						//	break;
							
						case '\\':	// reverse solidus
							s += "\\\\";
							break;
							
						case '\b':	// bell
							s += "\\b";
							break;
							
						case '\f':	// form feed
							s += "\\f";
							break;
							
						case '\n':	// newline
							s += "\\n";
							break;
							
						case '\r':	// carriage return
							s += "\\r";
							break;
							
						case '\t':	// horizontal tab
							s += "\\t";
							break;
							
						default:	// everything else
							
							// check for a control character and escape as unicode
							if ( ch < ' ' ) {
								// get the hex digit(s) of the character (either 1 or 2 digits)
								var hexCode:String = ch.charCodeAt( 0 ).toString( 16 );
								
								// ensure that there are 4 digits by adjusting
								// the # of zeros accordingly.
								var zeroPad:String = hexCode.length == 2 ? "00" : "000";
								
								// create the unicode escape sequence with 4 hex digits
								s += "\\u" + zeroPad + hexCode;
							} else {
							
								// no need to do any special encoding, just pass-through
								s += ch;
								
							}
					}	// end switch
					
				}	// end for loop
			}			
			
			return "\"" + s + "\"";
		}
		
		/**
		 * Converts an array to it's JSON string equivalent
		 *
		 * @param a The array to convert
		 * @return The JSON string representation of <code>a</code>
		 */
		private function arrayToString( a:Array ):String 
		{
			var i:int = 0;
			
			if (mUseByteArray)
			{				
				outputBA.writeByte(ARRAY_START);
				var hasElement:Boolean = false;
				// loop over the elements in the array and add their converted
				// values to the string
				for ( i = 0; i < a.length; i++ ) {
					// when the length is 0 we're adding the first element so
					// no comma is necessary
					if ( hasElement ) {
						// we've already added an element, so add the comma separator
						outputBA.writeByte(COMMA);
					}
					hasElement = true;
					// convert the value to a string
					convertToString( a[i] );	
				}
				
				// KNOWN ISSUE:  In ActionScript, Arrays can also be associative
				// objects and you can put anything in them, ie:
				//		myArray["foo"] = "bar";
				//
				// These properties aren't picked up in the for loop above because
				// the properties don't correspond to indexes.  However, we're
				// sort of out luck because the JSON specification doesn't allow
				// these types of array properties.
				//
				// So, if the array was also used as an associative object, there
				// may be some values in the array that don't get properly encoded.
				//
				// A possible solution is to instead encode the Array as an Object
				// but then it won't get decoded correctly (and won't be an
				// Array instance)
				outputBA.writeByte(ARRAY_END);
				// close the array and return it's string value
				return null;
				
			}
			else
			{
				// create a string to store the array's jsonstring value
				var s:String = "";
				
				// loop over the elements in the array and add their converted
				// values to the string
				for ( i= 0; i < a.length; i++ ) {
					// when the length is 0 we're adding the first element so
					// no comma is necessary
					if ( s.length > 0 ) {
						// we've already added an element, so add the comma separator
						s += ","
					}
					
					// convert the value to a string
					s += convertToString( a[i] );	
				}
				
				// KNOWN ISSUE:  In ActionScript, Arrays can also be associative
				// objects and you can put anything in them, ie:
				//		myArray["foo"] = "bar";
				//
				// These properties aren't picked up in the for loop above because
				// the properties don't correspond to indexes.  However, we're
				// sort of out luck because the JSON specification doesn't allow
				// these types of array properties.
				//
				// So, if the array was also used as an associative object, there
				// may be some values in the array that don't get properly encoded.
				//
				// A possible solution is to instead encode the Array as an Object
				// but then it won't get decoded correctly (and won't be an
				// Array instance)

				// close the array and return it's string value
				return "[" + s + "]";
			}
		}
		
		/**
		 * Converts an object to it's JSON string equivalent
		 *
		 * @param o The object to convert
		 * @return The JSON string representation of <code>o</code>
		 */
		private function objectToString( o:Object ):String
		{
			var classInfo:XML = null;
			var value:Object;
			var key:String = null;
			var v:XML = null;
			if (mUseByteArray)
			{
				
				outputBA.writeByte(OBJECT_START);
				
				// determine if o is a class instance or a plain object
				classInfo = describeType( o );
				return null;
				if ( classInfo.@name.toString() == "Object" )
				{
					// the value of o[key] in the loop below - store this 
					// as a variable so we don't have to keep looking up o[key]
					// when testing for valid values to convert
					value=null
					
					// loop over the keys in the object and add their converted
					// values to the string
					var hasElement:Boolean = false;
					for ( key in o )
					{
						// assign value to a variable for quick lookup
						value = o[key];
						
						// don't add function's to the JSON string
						if ( value is Function )
						{
							// skip this key and try another
							continue;
						}
						
						// when the length is 0 we're adding the first item so
						// no comma is necessary
						if ( hasElement ) {
							// we've already added an item, so add the comma separator
							outputBA.writeByte(COMMA);
						}
						hasElement = true;
						escapeString( key );
						outputBA.writeByte(COLON);
						convertToString( value );
					}
				}
				else // o is a class instance
				{
					// Loop over all of the variables and accessors in the class and 
					// serialize them along with their values.
					var hasElement2:Boolean = false;
					for each ( v in classInfo..*.( 
						name() == "variable"
						||
						( 
							name() == "accessor"
							// Issue #116 - Make sure accessors are readable
							&& attribute( "access" ).charAt( 0 ) == "r" ) 
						) )
					{
						// Issue #110 - If [Transient] metadata exists, then we should skip
						if ( v.metadata && v.metadata.( @name == "Transient" ).length() > 0 )
						{
							continue;
						}
						
						// When the length is 0 we're adding the first item so
						// no comma is necessary
						if ( hasElement2 ) {
							// We've already added an item, so add the comma separator
							outputBA.writeByte(COMMA);
						}
						hasElement2 = true;
						escapeString( v.@name.toString() )
						outputBA.writeByte(COLON);
						convertToString( o[ v.@name ] );
					}
					
				}
				outputBA.writeByte(OBJECT_END);
				return null;
				
			}
			else
			{
				// create a string to store the object's jsonstring value
				var s:String = "";
				
				
				// determine if o is a class instance or a plain object
				classInfo = describeType( o );
				if ( classInfo.@name.toString() == "Object" )
				{
					// the value of o[key] in the loop below - store this 
					// as a variable so we don't have to keep looking up o[key]
					// when testing for valid values to convert
					value = null;
					// loop over the keys in the object and add their converted
					// values to the string
					for ( key in o )
					{
						// assign value to a variable for quick lookup
						value = o[key];
						
						// don't add function's to the JSON string
						if ( value is Function )
						{
							// skip this key and try another
							continue;
						}
						
						// when the length is 0 we're adding the first item so
						// no comma is necessary
						if ( s.length > 0 ) {
							// we've already added an item, so add the comma separator
							s += ","
				
						}
						
						s += escapeString( key );
						s += ":";
				
						s += convertToString( value );
					}
				}
				else // o is a class instance
				{
					// Loop over all of the variables and accessors in the class and 
					// serialize them along with their values.
					for each ( v in classInfo..*.( 
						name() == "variable"
						||
						( 
							name() == "accessor"
							// Issue #116 - Make sure accessors are readable
							&& attribute( "access" ).charAt( 0 ) == "r" ) 
						) )
					{
						// Issue #110 - If [Transient] metadata exists, then we should skip
						if ( v.metadata && v.metadata.( @name == "Transient" ).length() > 0 )
						{
							continue;
						}
						
						// When the length is 0 we're adding the first item so
						// no comma is necessary
						if ( s.length > 0 ) {
							// We've already added an item, so add the comma separator
							s += ","
				
						}
						
						s += escapeString( v.@name.toString() )
						s += ":";
				
						s += convertToString( o[ v.@name ] );
					}
					
				}
				
				return "{" + s + "}";
			}
		}

		
	}
	
}
