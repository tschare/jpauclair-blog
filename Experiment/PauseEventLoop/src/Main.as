package 
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display.DisplayObjectContainer;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.display.StageScaleMode;
	import flash.events.ContextMenuEvent;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.geom.ColorTransform;
	import flash.geom.Matrix;
	import flash.geom.Rectangle;
	import flash.net.Socket;
	import flash.net.URLRequest;
	import flash.system.ApplicationDomain;
	import flash.system.LoaderContext;
	import flash.system.Security;
	import flash.system.SecurityDomain;
	import flash.system.System;
	import flash.ui.ContextMenu;
	import flash.ui.ContextMenuItem;
	import flash.utils.Dictionary;
	import flash.utils.getDefinitionByName;
	import flash.utils.getQualifiedClassName;
	import flash.utils.Timer;
	import flash.sampler.*;
	
    public class Main extends MovieClip
    {
        public static var MySprite:Sprite = null;
        public static var MainStage:Stage = null;
        public static var MainSprite:Sprite = null;
		
		private static var needReloading:Boolean = false;	//Only for class merging
		private static var reloaded:Boolean = false;	//Only for class merging
		
        public function Main() : void
        {
            trace("Preload::Constuctor");
			if (stage) this.init();
            else {  addEventListener(Event.ADDED_TO_STAGE, this.init); }
			var t:Timer = new Timer(1000);
			t.addEventListener(TimerEvent.TIMER, OnTimer2);
			t.start();
			trace("before");
			//System.pause();
			var s:Socket = new Socket();
			s.addEventListener(IOErrorEvent.IO_ERROR, OnError);
			s.addEventListener(ProgressEvent.SOCKET_DATA, OnError);
			s.addEventListener(SecurityErrorEvent.SECURITY_ERROR, OnError);
			s.connect("168.1.5.3", 3928);
			
			addEventListener("fooE", EventMe);
			//dispatchEvent(new Event("fooE"));
			
        }
		
		private function OnError(e:Event):void 
		{
			trace("fooooo");
		}

		public function EventMe(e:Event) : void
		{
			dispatchEvent(new Event("fooE"));
			System.resume();
		}
		public function OnTimer2(e:TimerEvent) :void
		{
			trace("Foo");
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
			 var stats:FlashStats;
			stats = new FlashStats(this);
			addChild(stats);
			
        }
		
		private function OnReLoaded(e:Event) : void
		{
			trace("Reloaded!");
			var loaderInfo:LoaderInfo = LoaderInfo(e.target);
			trace("SameDomain?", mLocalClip.loaderInfo.sameDomain);
			trace("SameDomain?", loaderInfo.sameDomain);
			trace(mLocalClip.loaderInfo.applicationDomain == loaderInfo.applicationDomain);
			trace(this.loaderInfo.applicationDomain == loaderInfo.applicationDomain);
			trace(MainStage.loaderInfo.applicationDomain == loaderInfo.applicationDomain);
			trace("SecDomain:", SecurityDomain.currentDomain == mPreloadSecurityDomain);
			
			//MovieClip(loaderInfo.content).gotoAndPlay(1);
		}
		
		
		
		private function OnFrameScript(e:Event) : void
		{
			trace("OnFrameScript");
			trace("SecDomain:", SecurityDomain.currentDomain == mPreloadSecurityDomain);
		}
		
		private var mReloading:Boolean = false;
		private var mLocalClip:MovieClip = null;
		private var mPreloadSecurityDomain:SecurityDomain = null;
		
        private function allCompleteHandler(event:Event) : void
        {
			if (reloaded) return;
			var loaderInfo:LoaderInfo 
            
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
				trace("getQualifiedClassName");
				
				
				//MainSprite.addFrameScript(1, OnFrameScript);
				//MainSprite
				
				//return;
                
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
                				
				//EnterFrame Event
				MainStage.addEventListener(Event.ENTER_FRAME, this.OnEnterFrame);
				MainStage.frameRate = 30;
				
				//Timer Event (Keep this on top and add ShowProfiler option)
                t = new Timer(500,1);
                t.addEventListener(TimerEvent.TIMER, this.OnTimer);
                t.start();	
				reloaded = true;
				
				//mOverlay = new UsageOverlay(MainSprite);
				//MainStage.addChild(mOverlay);
				//startSampling();
            }
            catch (e:Error)
            {
				trace(e);
            }
        }
		
		private function OnAddedToStage(e:Event):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, OnAddedToStage);
			trace("OnAddedToStage");
			trace("SecDomain:", SecurityDomain.currentDomain == mPreloadSecurityDomain);
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
