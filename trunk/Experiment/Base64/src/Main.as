package 
{
	import com.gskinner.utils.PerformanceTest;
	import flash.display.Sprite;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;
	import Frima.Util.Console;
	
	/**
	 * ...
	 * @author jpauclair
	 */
	public class Main extends Sprite 
	{
		private var _testString:ByteArray = Main.WriteString();
		private var outputField:TextField;
		private var ITERATIONS:int = 500;
		public function Main():void 
		{
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}
		private function out(str:String):void
		{
			outputField.appendText(str + "\n");
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			// entry point
			
			outputField = new TextField();
			outputField.width = stage.stageWidth;
			outputField.height = stage.stageHeight;
 
			addChild(outputField);			
			stage.scaleMode = StageScaleMode.NO_SCALE;

			
			var perfTest:PerformanceTest = PerformanceTest.getInstance();	
			perfTest.out = out;
			
			
			//perfTest.testFunction(EncodeBase, ITERATIONS, "test base 64", "");			
			perfTest.testFunction(EncodeJPTest, ITERATIONS, "test JP", "");			
			
			addChild(Console.GetInstance());
			
			
			//Console.Trace(Base64.encode(_testString).toString());
			Console.Trace(Base64JP.encode(_testString).length.toString());
			//Console.Trace(Base64JP.encode(_testString).toString());
			Console.GetInstance().y = 200;
		}
		private function EncodeBase():void
		{
			Base64.encode(_testString);
		}		
		private function EncodeJPTest():void
		{
			Base64JP.encode(_testString);
		}
		
		private static function WriteString() : ByteArray
		{
			var result:ByteArray = new ByteArray();
			//result.writeUTFBytes("j'ai un petit text avec des chiffre76 ! 9");
			//Console.Trace("aidhaSB1biBwZXRpdCB0ZXh0IGF2ZWMgZGVzIGNoaWZmcmU3NiAhIDk=");
			//aidhaSB1biBwZXRpdCB0ZXh0IGF2ZWMgZGVzIGNoaWZmcmU3NiAhIDk=
			result.position = 0
			//return result;
			result.writeUTFBytes("The term Base64 refers to a specific MIME content transfer encoding. It is also used as a generic term for any similar encoding scheme that encodes binary data by treating it numerically and translating it into a base 64 representation. The particular choice of base is due to the history of character set encoding: one can choose a set of 64 characters that is both part of the subset common to most encodings, and also printable. This combination leaves the data unlikely to be modified in transit through systems, such as email, which were traditionally not 8-bit clean."+
			"The precise choice of characters is difficult. The earliest instances of this type of encoding were created for dialup communication between systems running the same OS - e.g. Uuencode for UNIX, BinHex for the TRS-80 (later adapted for the Macintosh) - and could therefore make more assumptions about what characters were safe to use. For instance, Uuencode uses uppercase letters, digits, and many punctuation characters, but no lowercase, since UNIX was sometimes used with terminals that did not support distinct letter case. Unfortunately for interoperability with non-UNIX systems, some of the punctuation characters do not exist in other traditional character sets. The MIME Base64 encoding replaces most of the punctuation characters with the lowercase letters, a reasonable requirement by the time it was designed." +
			"The precise choice of characters is difficult. The earliest instances of this type of encoding were created for dialup communication between systems running the same OS - e.g. Uuencode for UNIX, BinHex for the TRS-80 (later adapted for the Macintosh) - and could therefore make more assumptions about what characters were safe to use. For instance, Uuencode uses uppercase letters, digits, and many punctuation characters, but no lowercase, since UNIX was sometimes used with terminals that did not support distinct letter case. Unfortunately for interoperability with non-UNIX systems, some of the punctuation characters do not exist in other traditional character sets. The MIME Base64 encoding replaces most of the punctuation characters with the lowercase letters, a reasonable requirement by the time it was designed." +
			"The precise choice of characters is difficult. The earliest instances of this type of encoding were created for dialup communication between systems running the same OS - e.g. Uuencode for UNIX, BinHex for the TRS-80 (later adapted for the Macintosh) - and could therefore make more assumptions about what characters were safe to use. For instance, Uuencode uses uppercase letters, digits, and many punctuation characters, but no lowercase, since UNIX was sometimes used with terminals that did not support distinct letter case. Unfortunately for interoperability with non-UNIX systems, some of the punctuation characters do not exist in other traditional character sets. The MIME Base64 encoding replaces most of the punctuation characters with the lowercase letters, a reasonable requirement by the time it was designed." +
			"The precise choice of characters is difficult. The earliest instances of this type of encoding were created for dialup communication between systems running the same OS - e.g. Uuencode for UNIX, BinHex for the TRS-80 (later adapted for the Macintosh) - and could therefore make more assumptions about what characters were safe to use. For instance, Uuencode uses uppercase letters, digits, and many punctuation characters, but no lowercase, since UNIX was sometimes used with terminals that did not support distinct letter case. Unfortunately for interoperability with non-UNIX systems, some of the punctuation characters do not exist in other traditional character sets. The MIME Base64 encoding replaces most of the punctuation characters with the lowercase letters, a reasonable requirement by the time it was designed." +
			"The precise choice of characters is difficult. The earliest instances of this type of encoding were created for dialup communication between systems running the same OS - e.g. Uuencode for UNIX, BinHex for the TRS-80 (later adapted for the Macintosh) - and could therefore make more assumptions about what characters were safe to use. For instance, Uuencode uses uppercase letters, digits, and many punctuation characters, but no lowercase, since UNIX was sometimes used with terminals that did not support distinct letter case. Unfortunately for interoperability with non-UNIX systems, some of the punctuation characters do not exist in other traditional character sets. The MIME Base64 encoding replaces most of the punctuation characters with the lowercase letters, a reasonable requirement by the time it was designed." +
			"The precise choice of characters is difficult. The earliest instances of this type of encoding were created for dialup communication between systems running the same OS - e.g. Uuencode for UNIX, BinHex for the TRS-80 (later adapted for the Macintosh) - and could therefore make more assumptions about what characters were safe to use. For instance, Uuencode uses uppercase letters, digits, and many punctuation characters, but no lowercase, since UNIX was sometimes used with terminals that did not support distinct letter case. Unfortunately for interoperability with non-UNIX systems, some of the punctuation characters do not exist in other traditional character sets. The MIME Base64 encoding replaces most of the punctuation characters with the lowercase letters, a reasonable requirement by the time it was designed." +
			"MIME Base64 uses A–Z, a–z, and 0–9 for the first 62 values. There are other similar systems, usually derived from Base64, that share this property but differ in the symbols chosen for the last two values; an example is UTF-7."+
			"Value	Char	Value	Char	Value	Char	Value	Char" +
			"0	A	16	Q	32	g	48	w"+
			"1	B	17	R	33	h	49	x"+
			"2	C	18	S	34	i	50	y"+
			"3	D	19	T	35	j	51	z"+
			"4	E	20	U	36	k	52	0"+
			"5	F	21	V	37	l	53	1"+
			"6	G	22	W	38	m	54	2"+
			"7	H	23	X	39	n	55	3"+
			"8	I	24	Y	40	o	56	4"+
			"9	J	25	Z	41	p	57	5"+
			"10	K	26	a	42	q	58	6"+
			"11	L	27	b	43	r	59	7"+
			"12	M	28	c	44	s	60	8"+
			"13	N	29	d	45	t	61	9"+
			"14	O	30	e	46	u	62	+"+
			"15	P	31	f	47	v	63	/");
			result.position = 0
			return result;
		}
	
	
	}
}