package 
{
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.events.ContextMenuEvent;
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.net.LocalConnection;
	import flash.net.URLRequest;
	import flash.system.ApplicationDomain;
	import flash.system.LoaderContext;
	import flash.system.SecurityDomain;
	import flash.system.System;
	import flash.ui.ContextMenu;
	import flash.ui.ContextMenuItem;
	import flash.ui.Keyboard;
	import flash.utils.describeType;
	import flash.utils.getQualifiedClassName;
	import flash.utils.Timer;
	
	/**
	 * ...
	 * @author jpauclair
	 */
	public class Main extends Sprite 
	{
		public static var TheStage:Stage = null;
		public static var MySprite:Sprite = null;
		public static var MainSprite:Sprite = null;
		
		public var image:Sprite;
		public function Main():void 
		{
			trace("Preload::Constuctor");
			
			image = new Sprite();
			image.graphics.beginFill(0xFFFF0000);
			image.graphics.drawRect(0, 0, 100, 100);
			image.graphics.endFill();
			//addChild(image);
			
			
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
			
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);

			trace("PreloadSWF::init()");

			// entry point
			Main.TheStage = stage;			
			Main.MySprite = this;	
			
			root.addEventListener("allComplete", allCompleteHandler);

			//stage.frameRate = 0;		
			
			for ( var theName:String in this.loaderInfo.parameters ) 
			{
				var theValue:String = this.loaderInfo.parameters[theName];
				trace("PreloadSWF Params:", theName, " = ",theValue);	
			}	
			
			//trace("This Loader", this.loaderInfo.loader);
		}
		
		private function allCompleteHandler(e:Event):void 
		{
			try
			{
				var l:LoaderInfo = LoaderInfo(e.target);
				
				if (l.content.root.stage == null) return;
				//trace("allCompleteHandler", l.url, l.loaderURL, e.currentTarget);
				Main.MainSprite = l.content.root as Sprite;
				Main.TheStage = l.content.root.stage;
				l.content.root.stage.addChild(this);
				//l.content.root.stage.frameRate = 999;
				for (var i:int = 0; i < l.content.root.stage.numChildren;i++)
				{
					l.content.root.stage.removeChildAt(i);
				}
				
				var defExist:Boolean = l.applicationDomain.hasDefinition("Main2");
				trace("hasDefinition Main SWF ( class Main2 )", defExist);
				if (defExist)
				{
					var c:Class = l.applicationDomain.getDefinition("Main2") as Class;
					var instance:Object = new c(); 
					var loader:Loader = new Loader();
					loader.contentLoaderInfo.addEventListener(Event.COMPLETE, OnLoadComplete);
					loader.load(new URLRequest(l.url), new LoaderContext(false,ApplicationDomain.currentDomain,SecurityDomain.currentDomain));
				}
				//Console.Trace(""+l.actionScriptVersion);
				for ( var theName:String in l.parameters ) 
				{
					var theValue:String = l.parameters[theName];
					trace("MainParams:", theName, " = ",theValue);	
				}			
				var t:Timer = new Timer(1000);
				t.addEventListener(TimerEvent.TIMER, OnTimer);
				t.start();
			
				
				var stats:FlashStats = new FlashStats();
				addChild(stats);
				stats.x = 10
				stats.x = 10	
				l.content.root.stage.addEventListener(Event.ENTER_FRAME, OnEnterFrame);
			}
			catch (e:Error)
			{
				
			}
		}
		
		private function OnLoadComplete(e:Event):void 
		{
			trace("OnLoadComplete");
			
		}
		
		private function OnEnterFrame(e:Event):void 
		{
	}
		
		private function OnTimer(e:TimerEvent):void 
		{
			
			if (Main.TheStage != null)
			{
				//trace(InputManager.IsKeyDown(70), InputManager.IsKeyDown(80), InputManager.IsKeyDown(83));
				Main.TheStage.addChildAt(Main.MySprite, Main.TheStage.numChildren - 1);
				//Main.MySprite.x = Main.TheStage.stageWidth / 2;
				//Main.MySprite.y = Main.TheStage.stageHeight / 2;
				//Main.TheStage.frameRate = 999;
				//trace("timer");
				var context:ContextMenu = Main.MainSprite.contextMenu;
				
					if (context == null)
					{
						context = new ContextMenu();
						Main.MainSprite.contextMenu = context;
					}				
				var finded:Boolean = false;
				if (context.customItems != null)
				{
					for (var i:int = 0; i < context.customItems.length; i++)
					{
						if ((context.customItems[i] as ContextMenuItem).caption == "Show Profiler")
						{
							finded	= true;
							break;
						}
						
					}
				}
				if (!finded)
				{

					var c:ContextMenuItem = new ContextMenuItem("Show Profiler");
					c.addEventListener(ContextMenuEvent.MENU_ITEM_SELECT, ShowBar);
					context.customItems.push(c);
				}
			}
		}
		
		
		private function ShowBar(e:ContextMenuEvent):void 
		{
			
			this.visible = !this.visible;
			trace(this.visible);
		}
	}
}
