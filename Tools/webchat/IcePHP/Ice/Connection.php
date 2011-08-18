<?php
// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
// Ice version 3.4.2
//
// <auto-generated>
//
// Generated from file `Connection.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

require_once 'Ice/ObjectAdapterF.php';
require_once 'Ice/Identity.php';
require_once 'Ice/Endpoint.php';

if(!class_exists('Ice_ConnectionInfo'))
{
    class Ice_ConnectionInfo
    {
        public function __construct($incoming=false, $adapterName='')
        {
            $this->incoming = $incoming;
            $this->adapterName = $adapterName;
        }

        public function __toString()
        {
            global $Ice__t_ConnectionInfo;
            return IcePHP_stringify($this, $Ice__t_ConnectionInfo);
        }

        public $incoming;
        public $adapterName;
    }

    $Ice__t_ConnectionInfo = IcePHP_defineClass('::Ice::ConnectionInfo', 'Ice_ConnectionInfo', false, $Ice__t_Object, null, array(
        array('incoming', $IcePHP__t_bool),
    
        array('adapterName', $IcePHP__t_string)));
}

if(!interface_exists('Ice_Connection'))
{
    interface Ice_Connection
    {
        public function close($force);
        public function createProxy($id);
        public function setAdapter($adapter);
        public function getAdapter();
        public function getEndpoint();
        public function flushBatchRequests();
        public function type();
        public function timeout();
        public function toString();
        public function getInfo();
    }

    $Ice__t_Connection = IcePHP_defineClass('::Ice::Connection', 'Ice_Connection', true, $Ice__t_Object, null, null);
}

if(!class_exists('Ice_IPConnectionInfo'))
{
    class Ice_IPConnectionInfo extends Ice_ConnectionInfo
    {
        public function __construct($incoming=false, $adapterName='', $localAddress='', $localPort=0, $remoteAddress='', $remotePort=0)
        {
            parent::__construct($incoming, $adapterName);
            $this->localAddress = $localAddress;
            $this->localPort = $localPort;
            $this->remoteAddress = $remoteAddress;
            $this->remotePort = $remotePort;
        }

        public function __toString()
        {
            global $Ice__t_IPConnectionInfo;
            return IcePHP_stringify($this, $Ice__t_IPConnectionInfo);
        }

        public $localAddress;
        public $localPort;
        public $remoteAddress;
        public $remotePort;
    }

    $Ice__t_IPConnectionInfo = IcePHP_defineClass('::Ice::IPConnectionInfo', 'Ice_IPConnectionInfo', false, $Ice__t_ConnectionInfo, null, array(
        array('localAddress', $IcePHP__t_string),
    
        array('localPort', $IcePHP__t_int),
    
        array('remoteAddress', $IcePHP__t_string),
    
        array('remotePort', $IcePHP__t_int)));
}

if(!class_exists('Ice_TCPConnectionInfo'))
{
    class Ice_TCPConnectionInfo extends Ice_IPConnectionInfo
    {
        public function __construct($incoming=false, $adapterName='', $localAddress='', $localPort=0, $remoteAddress='', $remotePort=0)
        {
            parent::__construct($incoming, $adapterName, $localAddress, $localPort, $remoteAddress, $remotePort);
        }

        public function __toString()
        {
            global $Ice__t_TCPConnectionInfo;
            return IcePHP_stringify($this, $Ice__t_TCPConnectionInfo);
        }
    }

    $Ice__t_TCPConnectionInfo = IcePHP_defineClass('::Ice::TCPConnectionInfo', 'Ice_TCPConnectionInfo', false, $Ice__t_IPConnectionInfo, null, null);
}

if(!class_exists('Ice_UDPConnectionInfo'))
{
    class Ice_UDPConnectionInfo extends Ice_IPConnectionInfo
    {
        public function __construct($incoming=false, $adapterName='', $localAddress='', $localPort=0, $remoteAddress='', $remotePort=0, $mcastAddress='', $mcastPort=0)
        {
            parent::__construct($incoming, $adapterName, $localAddress, $localPort, $remoteAddress, $remotePort);
            $this->mcastAddress = $mcastAddress;
            $this->mcastPort = $mcastPort;
        }

        public function __toString()
        {
            global $Ice__t_UDPConnectionInfo;
            return IcePHP_stringify($this, $Ice__t_UDPConnectionInfo);
        }

        public $mcastAddress;
        public $mcastPort;
    }

    $Ice__t_UDPConnectionInfo = IcePHP_defineClass('::Ice::UDPConnectionInfo', 'Ice_UDPConnectionInfo', false, $Ice__t_IPConnectionInfo, null, array(
        array('mcastAddress', $IcePHP__t_string),
    
        array('mcastPort', $IcePHP__t_int)));
}
?>