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
	import flash.utils.getTimer;
	import flash.utils.Timer;
	import flash.sampler.*;
	
    public class Main extends MovieClip
    {
        public static var MySprite:Sprite = null;
        public static var MainStage:Stage = null;
        public static var MainSprite:Sprite = null;
		
		private static var needReloading:Boolean = false;	//Only for class merging
		private static var reloaded:Boolean = false;	//Only for class merging
		private var mSocket:Socket;
        public function Main() : void
        {
			
            trace("Preload::Constuctor");
			if (stage) this.init();
            else {  addEventListener(Event.ADDED_TO_STAGE, this.init); }
			var t:Timer = new Timer(1000);
			t.addEventListener(TimerEvent.TIMER, OnTimer2);
			t.start();
			trace("before");
			System.pause();
			this.stage.frameRate = 999;
			mSocket = new Socket();
			mSocket.addEventListener(IOErrorEvent.IO_ERROR, OnError);
			mSocket.addEventListener(ProgressEvent.SOCKET_DATA, OnError);
			mSocket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, OnError);
			//s.connect("168.1.5.3", 3928);
			timerCallBack = getTimer();
			mSocket.connect("127.0.0.1", 0);
			
			addEventListener("fooE", EventMe);
			//dispatchEvent(new Event("fooE"));
			
        }
		private var timerCallBack:int = 0;
		private function OnError(e:Event):void 
		{
			timerCallBack = getTimer() - timerCallBack;
			trace("Crash! resume", timerCallBack);
			timerCallBack = getTimer();
			System.resume();
			mSocket = new Socket();
			mSocket.addEventListener(IOErrorEvent.IO_ERROR, OnError);
			mSocket.addEventListener(ProgressEvent.SOCKET_DATA, OnError);
			mSocket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, OnError);
			
			mSocket.connect("127.0.0.1", 0);			
			System.pause();
			//System.resume();
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
								
				//Show some stats
                stats = new FlashStats(MainStage);
                addChild(stats);
                				
				//EnterFrame Event
				MainStage.addEventListener(Event.ENTER_FRAME, this.OnEnterFrame);
				MainStage.frameRate = 2;
				
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
		
		private function OnAddedToStage(e:Event):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, OnAddedToStage);
			trace("OnAddedToStage");
			trace("SecDomain:", SecurityDomain.currentDomain == mPreloadSecurityDomain);
		}
		
		
		private var frameCounter:int = 0;
		private function OnEnterFrame(e:Event):void
		{
			// Anything you want
			trace("EnterFrame", frameCounter++);
		}
				
		private var timerCounter:int = 0;
        private function OnTimer(event:TimerEvent) : void
        {
			trace("timer", timerCounter++);
        }

    }
}
