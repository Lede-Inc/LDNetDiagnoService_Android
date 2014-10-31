package com.netease.LDNetDiagnoService;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import com.netease.LDNetDiagnoService.LDNetAsyncTaskEx;


import com.netease.LDNetDiagnoService.LDNetDiagnoListener;
import com.netease.LDNetDiagnoService.LDNetPing.LDNetPingListener;
import com.netease.LDNetDiagnoService.LDNetTraceRoute.LDNetTraceRouteListener;

/**
 * 网络诊断服务
 * 通过对制定域名进行ping诊断和traceroute诊断收集诊断日志
 * @author panghui
 *
 */
public class LDNetDiagnoService extends LDNetAsyncTaskEx<String, String, String>  implements LDNetPingListener, LDNetTraceRouteListener {
	private String _appCode; //客户端标记
	private String _appName;
	private String _appVersion;
	private String _UID; //用户ID
	private String _deviceID; //客户端机器ID，如果不传入会默认取API提供的机器ID
	private String _dormain; //接口域名
	private String _carrierName;
	private String _ISOCountryCode;
	private String _MobileCountryCode;
	private String _MobileNetCode;
	
	private StringBuilder _logInfo;
	private LDNetPing _netPinger; //监控ping命令的执行时间
	private LDNetTraceRoute _traceRouter; //监控ping模拟traceroute的执行过程
	private boolean _isRunning;
	
	private LDNetDiagnoListener _netDiagnolistener; //将监控日志上报到前段页面
	 
	
	/**
	 * 初始化网络诊断服务
	 * @param theAppCode
	 * @param theDeviceID
	 * @param theUID
	 * @param theDormain
	 */
	public LDNetDiagnoService(String theAppCode,
			String theAppName,
			String theAppVersion,
			String theUID, 
			String theDeviceID,
			String theDormain,
			String theCarrierName,
			String theISOCountryCode,
			String theMobileCountryCode,
			String theMobileNetCode, 
			LDNetDiagnoListener theListener){
		super();
		this._appCode = theAppCode;
		this._appName = theAppName;
		this._appVersion = theAppVersion;
		this._UID = theUID;
		this._deviceID = theDeviceID;
		this._dormain = theDormain;
		this._carrierName = theCarrierName;
		this._ISOCountryCode = theISOCountryCode;
		this._MobileCountryCode = theMobileCountryCode;
		this._MobileNetCode  = theMobileNetCode;
		this._netDiagnolistener = theListener;
		//
		this._isRunning = false;
	}
	

    @Override
    protected String doInBackground(String... params) {
    	// TODO Auto-generated method stub
    	return startNetDiagnosis();
    }

    
    @Override
    protected void onPostExecute(String result) {
    	super.onPostExecute(result);
    	if (_netDiagnolistener != null) {
    		_netDiagnolistener.OnNetDiagnoFinished(result);
    	}
    }


    @Override
    protected void onProgressUpdate(String... values) {
    	// TODO Auto-generated method stub
    	super.onProgressUpdate(values);
    	if (_netDiagnolistener != null) {
    		_netDiagnolistener.OnNetDiagnoUpdated(values[0]);
    	}
    }


	/**
	 * 输出关于应用、机器、网络诊断的基本信息
	 */
	private void recordCurrentAppVersion() {
	    //输出应用版本信息和用户ID
	    recordStepInfo("应用code:\t"+_appCode);
	    recordStepInfo("应用名称:\t"+ this._appName);
	    recordStepInfo("应用版本:\t"+ this._appVersion);
	    recordStepInfo("用户id:\t"+ _UID);
	    
	    //输出机器信息
	    recordStepInfo("机器类型:\t" +  android.os.Build.MANUFACTURER + ":" + android.os.Build.BRAND + ":" + android.os.Build.MODEL);
	    recordStepInfo("系统版本:\t" +  android.os.Build.VERSION.RELEASE);
	    recordStepInfo("机器ID:\t" + _deviceID);	    
	    
	    //运营商信息
	    if (true) {
	        recordStepInfo("运营商:\t" + this._carrierName);
	        recordStepInfo("ISOCountryCode:\t" + this._ISOCountryCode);
	        recordStepInfo("MobileCountryCode:\t" + this._MobileCountryCode);
	        recordStepInfo("MobileNetworkCode:\t" + this._MobileNetCode);
	    }
	}


	/**
	 * 开始诊断网络
	 */
	public String startNetDiagnosis(){
		if(this._dormain == null || this._dormain.equalsIgnoreCase("")) return "";
		
		this._isRunning = true;
		_logInfo = new StringBuilder(256);
		recordStepInfo("开始诊断...");
	    recordCurrentAppVersion();
	    
	    //诊断ping信息, 同步过程
	    recordStepInfo("\n\n诊断域名 "+ _dormain +"...");
	    recordStepInfo("\n开始ping...");
	    _netPinger = new LDNetPing(this, 3);
	    _netPinger.exec(_dormain);
	    
	    
	    //开始诊断traceRoute
	    recordStepInfo("\n开始traceroute...");
	    _traceRouter = new LDNetTraceRoute(this);
	    _traceRouter.startTraceRoute(_dormain);
	    return _logInfo.toString(); 
	}

	/**
	 * 停止诊断网络
	 */
	public void stopNetDialogsis(){
	    if(_isRunning){
	        if(_netPinger != null){
	            _netPinger = null;
	        }
	        
	        if(_traceRouter != null) {
	            _traceRouter = null;
	        }
	        
	        this.cancel(true);
	        _isRunning = false;
	    }
	}


	/**
	 * 打印整体loginInfo；
	 */
	public void printLogInfo() {
		System.out.print(_logInfo);
	}



	/**
	 * 如果调用者实现了stepInfo接口，输出信息
	 * @param stepInfo
	 */
	private void recordStepInfo(String stepInfo){
		_logInfo.append(stepInfo+"\n");
		publishProgress(stepInfo+"\n");
	}

	/**
	 * traceroute 消息跟踪
	 */
	@Override
	public void OnNetTraceFinished() {
		recordStepInfo("\n网络诊断结束\n");
	}


	@Override
	public void OnNetTraceUpdated(String log) {
		this.recordStepInfo(log);
	}


	/**
	 * ping 消息跟踪
	 */
	@Override
	public void OnNetPingFinished(String log) {
		this.recordStepInfo(log);
	}
	
	private static final int CORE_POOL_SIZE = 4;
    private static final int MAXIMUM_POOL_SIZE = 10;
    private static final int KEEP_ALIVE = 10;
    
    private static final BlockingQueue<Runnable> sWorkQueue = new LinkedBlockingQueue<Runnable>(2);
    private static final ThreadFactory sThreadFactory = new ThreadFactory() {
    	private final AtomicInteger mCount = new AtomicInteger(1);
    	public Thread newThread(Runnable r) {
    		Thread t = new Thread(r, "Trace #" + mCount.getAndIncrement());
    		t.setPriority(Thread.MIN_PRIORITY);
    		return t;
    	}
    };
    
    private static final ThreadPoolExecutor sExecutor = new ThreadPoolExecutor(CORE_POOL_SIZE, MAXIMUM_POOL_SIZE, KEEP_ALIVE, TimeUnit.SECONDS,
	    sWorkQueue, sThreadFactory);
    @Override
    protected ThreadPoolExecutor getThreadPoolExecutor() {
    	// TODO Auto-generated method stub
    	return sExecutor;
    }
	
}
