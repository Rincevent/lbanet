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
// Generated from file `Current.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

require_once 'Ice/ObjectAdapterF.php';
require_once 'Ice/ConnectionF.php';
require_once 'Ice/Identity.php';

if(!isset($Ice__t_Context))
{
    $Ice__t_Context = IcePHP_defineDictionary('::Ice::Context', $IcePHP__t_string, $IcePHP__t_string);
}

if(!class_exists('Ice_OperationMode'))
{
    class Ice_OperationMode
    {
        const Normal = 0;
        const Nonmutating = 1;
        const Idempotent = 2;
    }

    $Ice__t_OperationMode = IcePHP_defineEnum('::Ice::OperationMode', array('Normal', 'Nonmutating', 'Idempotent'));
}

if(!class_exists('Ice_Current'))
{
    class Ice_Current
    {
        public function __construct($adapter=null, $con=null, $id=null, $facet='', $operation='', $mode=Ice_OperationMode::Normal, $ctx=null, $requestId=0)
        {
            $this->adapter = $adapter;
            $this->con = $con;
            $this->id = is_null($id) ? new Ice_Identity : $id;
            $this->facet = $facet;
            $this->operation = $operation;
            $this->mode = $mode;
            $this->ctx = $ctx;
            $this->requestId = $requestId;
        }

        public function __toString()
        {
            global $Ice__t_Current;
            return IcePHP_stringify($this, $Ice__t_Current);
        }

        public $adapter;
        public $con;
        public $id;
        public $facet;
        public $operation;
        public $mode;
        public $ctx;
        public $requestId;
    }

    $Ice__t_Current = IcePHP_defineStruct('::Ice::Current', 'Ice_Current', array(
        array('adapter', $Ice__t_ObjectAdapter), 
        array('con', $Ice__t_Connection), 
        array('id', $Ice__t_Identity), 
        array('facet', $IcePHP__t_string), 
        array('operation', $IcePHP__t_string), 
        array('mode', $Ice__t_OperationMode), 
        array('ctx', $Ice__t_Context), 
        array('requestId', $IcePHP__t_int)));
}
?>
