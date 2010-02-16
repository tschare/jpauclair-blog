package 
{
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.system.ApplicationDomain;
	import flash.utils.getDefinitionByName;
	import flash.utils.getQualifiedClassName;
	
	/**
	 * ...
	 * @author jpauclair
	 */
	public class Main2 extends Sprite 
	{
		public var image:Sprite;
		public function Main2():void 
		{
			image = new Sprite();
			image.graphics.beginFill(0xFF0000FF);
			image.graphics.drawRect(100, 100, 100, 100);
			image.graphics.endFill();
			addChild(image);	
			
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);

			stage.addEventListener(Event.ENTER_FRAME, OnEnterFrame);
		}
		
		private function OnEnterFrame(e:Event):void 
		{
		//	trace("Main EnterFrame");// , this.stage.numChildren);
		}
		
	}
	
}