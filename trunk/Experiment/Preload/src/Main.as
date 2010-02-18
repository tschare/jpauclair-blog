package 
{
	import flash.display.DisplayObject;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.events.ContextMenuEvent;
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.net.URLRequest;
	import flash.system.ApplicationDomain;
	import flash.system.LoaderContext;
	import flash.system.SecurityDomain;
	import flash.ui.ContextMenu;
	import flash.ui.ContextMenuItem;
	import flash.utils.getDefinitionByName;
	import flash.utils.Timer;
	import nl.demonsters.debugger.MonsterDebugger;
	
    public class Main extends Sprite
    {
        public static var MySprite:Sprite = null;
        public static var MainStage:Stage = null;
        public static var MainSprite:Sprite = null;
		
		private static var needReloading:Boolean = false;	//Only for class merging
		private static var reloaded:Boolean = false;	//Only for class merging
		private var debugger:MonsterDebugger;
		
        public function Main() : void
        {
            trace("Preload::Constuctor");
            
			if (stage) this.init();
            else {  addEventListener(Event.ADDED_TO_STAGE, this.init); }
        }

        private function init(event:Event = null) : void
        {
            trace("PreloadSWF::init()");
            removeEventListener(Event.ADDED_TO_STAGE, this.init);
			
            MySprite = this;
            root.addEventListener("allComplete", this.allCompleteHandler);

			//Trace Preloaded Params
			var paramName:String = null;
            var paramValue:String = null;
            for (paramName in this.loaderInfo.parameters)
            {
                paramValue = this.loaderInfo.parameters[paramName];
                trace("PreloadSWF Params:", paramName, " = ", paramValue);
            }
        }
		
        private function allCompleteHandler(event:Event) : void
        {
			if (reloaded) return;
			
            var loaderInfo:LoaderInfo;
            var theName:String;
            var t:Timer;
            var stats:FlashStats;
            var theValue:String;
			
            try
            {
                loaderInfo = LoaderInfo(event.target);
                
				if (loaderInfo.content.root.stage == null) { return; }
				
                MainSprite = loaderInfo.content.root as Sprite;
                MainStage = MainSprite.stage;

                
				//Remove all previous sprite
				if (needReloading)
				{
					while (MainStage.numChildren > 0)
					{
						var obj:Sprite = MainStage.removeChildAt(0) as Sprite;
						obj.mouseChildren = false;
						obj.mouseEnabled = false;
						obj.visible = false;
					}
				}
				
				//Add our Sprite
                MainStage.addChild(this);				
				

				
				
				if (needReloading) //Only for class merging
				{
					//Re-instanciate it in our domain
					var loader:Loader = new Loader();
					loader.contentLoaderInfo.addEventListener(Event.COMPLETE, OnReLoadCompleted);
					loader.loadBytes(MainStage.loaderInfo.bytes, new LoaderContext(false, ApplicationDomain.currentDomain));// . SecurityDomain.currentDomain));
				}
                
				//Trace all paramaters loaded in Main application
				var paramName:String;
                var paramValue:String;
                while (paramName in loaderInfo.parameters)
                {
                    paramValue = loaderInfo.parameters[paramName];
                    trace("Main Params:", paramName, " = ", theValue);
                }

				
				//Show some stats
                stats = new FlashStats(MainStage);
                addChild(stats);
                
				//Attach an external debugger
				debugger = new MonsterDebugger(MainStage);
				
				//EnterFrame Event
				MainStage.addEventListener(Event.ENTER_FRAME, this.OnEnterFrame);
				MainStage.frameRate = 30;
				
				//Timer Event (Keep this on top and add ShowProfiler option)
                t = new Timer(500,1);
                t.addEventListener(TimerEvent.TIMER, this.OnTimer);
                t.start();	
				reloaded = true;
				
            }
            catch (e:Error)
            {
				trace(e);
            }
        }
		
		
		private function OnReLoadCompleted(e:Event):void 
		{
			var exist:Boolean = ApplicationDomain.currentDomain.hasDefinition("AnyClass");
			trace("Reloaded, class exist?", exist);
			MainStage.addChild(e.target.content)
			//If you have redefined a class that is in the Main app domain, yours should take over.
			reloaded = true;
		}
		
		private function OnEnterFrame(e:Event):void
		{
			// Anything you want
		}

        private function ShowBar(event:ContextMenuEvent) : void
        {
            this.visible = !this.visible;
            trace(this.visible);
        }
		
        private function OnTimer(event:TimerEvent) : void
        {
            var menu:ContextMenu = null;
            var alreadyInMenu:Boolean = false;
            var i:int = 0;
            var menuItem:ContextMenuItem = null;
            if (MainStage != null)
            {
				
                MainStage.addChildAt(MySprite, (MainStage.numChildren - 1));
                menu = MainSprite.contextMenu;
                if (menu == null)
                {
                    menu = new ContextMenu();
                    MainSprite.contextMenu = menu;
                }
                alreadyInMenu = false;
                if (menu.customItems != null)
                {
                    i = 0;
                    while (i < menu.customItems.length)
                    {
                        
                        if ((menu.customItems[i] as ContextMenuItem).caption == "Show Profiler")
                        {
                            alreadyInMenu = true;
                            break;
                        }
                        i = i + 1;
                    }
                }
                if (!alreadyInMenu)
                {
                    menuItem = new ContextMenuItem("Show Profiler");
                    menuItem.addEventListener(ContextMenuEvent.MENU_ITEM_SELECT, this.ShowBar);
                    menu.customItems.push(menuItem);
                }
            }
        }

    }
}
