package  
{
	/**
	 * ...
	 * @author 
	 */
	public class SimpleClass2
	{
		public var firstName:String = "John";
		public var lastName:String = "Smith";
		public var age:int = 25;
		public var adress:Object = { streetAddress:"21 2nd Street", 
									 city:"New York",
									 state: "NY",
									 postalCode: "10021" };
		public var  phoneNumber:Array = [	{type:"home", number:"212 555-1234"},
											{type:"fax", number:"646 555-4567" } ];
		public function SimpleClass2() 
		{
			
		}

		/*
		{
			 "firstName": "John",
			 "lastName": "Smith",
			 "age": 25,
			 "address": {
				 "streetAddress": "21 2nd Street",
				 "city": "New York",
				 "state": "NY",
				 "postalCode": "10021"
			 },
			 "phoneNumber": [
				 { "type": "home", "number": "212 555-1234" },
				 { "type": "fax", "number": "646 555-4567" }
			 ]
		 }
 
		 * */
	}

}