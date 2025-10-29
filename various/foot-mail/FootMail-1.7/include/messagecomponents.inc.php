<?php
	class message_components {

	/*
	 * Original IMAP message scanner (see below).
	 *
	 * I (BSC) changed a bit in this file ...
	 */
        /**
         *+----------------------------------------------------------------------------------------------------+
         *| IMAP message scanner - scans information provided by imap_fetchstructure()                         |
         *|                                                                                                    |
         *| Author: Richard York                                                                               |
         *| mailto:richy at smilingsouls.net                                                                    |
         *| http://www.smilingsouls.net                                                                        |
         *|                                                                                                    |
         *| (c) Copyright 2004, Richard York, All Rights Reseverd                                              |
         *+----------------------------------------------------------------------------------------------------+
         **
        */

        var $mailbox;           // (resource)              Imap stream

        var $data_types;        // (array)(string)         Various message part types
        var $encoding_types;    // (array)(string)         Various encoding types

        // first array uses message id as key
        // nested array is offset corresponding with the number of parts
        
        var $structure;         // (array)(object)         Contains the complete body structure
        var $pid;               // (array)(array)(str)     part id
        var $file_type;         // (array)(array)(str)     mime type
        var $disposition;       // (array)(array)(str)     inline | attachment
        var $fsize;             // (array)(array)(int)     part size in bytes
        var $encoding;          // (array)(array)(str)     message encoding
        var $fname;             // (array)(array)(str)     original file name
        var $inline_id;         // (array)(array)(str)     string containing the id for multipart/related
        var $has_attachments;   // (array)(array)(bool)

        /** 
         * CONSTRUCTOR
         *
         * void message_components(resource imap stream)
         **
        */
        
        function message_components($mailbox)
        {
            $this->data_types = array();

                $this->data_types[0] = 'text';
                $this->data_types[1] = 'multipart';
                $this->data_types[2] = 'message';
                $this->data_types[3] = 'application';
                $this->data_types[4] = 'audio';
                $this->data_types[5] = 'image';
                $this->data_types[6] = 'video';
                $this->data_types[7] = 'other';

            $this->encoding_types = array();

                $this->encoding_types[0] = '7bit';
                $this->encoding_types[1] = '8bit';
                $this->encoding_types[2] = 'binary';
                $this->encoding_types[3] = 'base64';
                $this->encoding_types[4] = 'quoted-printable';
                $this->encoding_types[5] = 'other';

            $this->mailbox = $mailbox;

            return;
        }
        
        /**
         * void fetch_structure(int message id[, array recursive subpart[, array recursive parent part id[, int recursive counter[, bool recursive is a sub part[, bool recursive skip part]]]]])
         * Indexes the structure of a message body.
         * 
         * Gather message information returned by imap_fetchstructure()
         * Recursively iterate through each parts array
         * Concatenate part numbers in the following format `1.1` each part id is separated by a period, each referring 
         * to a part or subpart of a multipart message.  Create part numbers as such that they are compatible with imap_fetchbody()
         **
        */

        function fetch_structure($mid, $sub_part = null, $sub_pid = null, $n = 0, $is_sub_part = false, $skip_part = false)
        {
            if (!is_array($sub_part))
            {
                $this->structure[$mid] = imap_fetchstructure($this->mailbox, $mid);
            }

            if (isset($this->structure[$mid]->parts) || is_array($sub_part))
            {
                if ($is_sub_part == false)
                {
                    $parts = $this->structure[$mid]->parts;
                }

                else
                {
                    $parts = $sub_part;
                    $n++;
                }

                for($p = 0, $i = 1; $p < count($parts); $n++, $p++, $i++)
                {
                    // Skip the following...
                    // Skip multipart/mixed!
                    // Skip subsequent multipart/alternative if this part is message/rfc822
                    // Skip multipart/related

                    $ftype        = (empty($parts[$p]->type))?           $this->data_types[0].'/'.strtolower($parts[$p]->subtype) : $this->data_types[$parts[$p]->type].'/'.strtolower($parts[$p]->subtype);
                    $encoding     = (empty($parts[$p]->encoding))?       $this->encoding_types[0] : $this->encoding_types[$parts[$p]->encoding];
                    $skip_next    = ($ftype == 'message/rfc822')?        true : false;

                    if ($ftype == 'multipart/mixed' || $skip_part == true && $ftype == 'multipart/alternative' || $ftype == 'multipart/related')
                    {
                        $n--;
                    }

                    else
                    {
                        $this->pid[$mid][$n]       = ($is_sub_part == false)? $i : $sub_pid.'.'.$i;
                        $this->file_type[$mid][$n] = $ftype;
                        $this->encoding[$mid][$n]  = $encoding;
                        $this->fsize[$mid][$n]     = (!isset($parts[$p]->bytes) || empty($parts[$p]->bytes))? 0 : $parts[$p]->bytes;

                        // Force inline disposition if none is present

			// BSC changed this part - filenames can also be
			// present in inline dispositions
                        if ($parts[$p]->ifdisposition == true)
                        {
                            $this->disposition[$mid][$n] = strtolower($parts[$p]->disposition);
                        }
                        else
                        {
                            $this->disposition[$mid][$n] = 'inline';
                        }

			// BSC: inlined filename?
			if (isset($parts[$p]->dparameters)) {
	                        $params = $parts[$p]->dparameters;

                                foreach ($params as $param)
                                {
                                      if(strtolower($param->attribute) == 'filename')
                                      {
                                           $this->fname[$mid][$n] = $param->value;
                                            break;
                                       }
                                }
			}

                        if ($parts[$p]->ifid == true)
                        {
                            $this->inline_id[$mid][$n] = $parts[$p]->id;
                        }
                    }

                    if (isset($parts[$p]->parts) && is_array($parts[$p]->parts))
                    {
                        $this->has_attachments[$mid][$n] = true;
                        $n = $this->fetch_structure($mid, $parts[$p]->parts, $this->pid[$mid][$n], $n, true, $skip_next);
                    }

                    else
                    {
                        $this->has_attachments[$mid][$n] = false;
                    }
                }

                if ($is_sub_part == true)
                {
                    return $n;
                }
            }

            // $parts is not an array... message is flat
            else
            {                    
                $this->pid[$mid][0] = 1;

                if (empty($this->structure[$mid]->type)) 
                {
                    $this->structure[$mid]->type        = (int) 0;
                }
                
                if (isset($this->structure[$mid]->subtype))
                {
                    $this->file_type[$mid][0]            = $this->data_types[$this->structure[$mid]->type].'/'.strtolower($this->structure[$mid]->subtype);
                }
            
                if (empty($this->structure[$mid]->encoding))
                {
                    $this->structure[$mid]->encoding    = (int) 0;
                }
                
                $this->encoding[$mid][0]                = $this->encoding_types[$this->structure[$mid]->encoding];

                if (isset($this->structure[$mid]->bytes))
                {
                    $this->fsize[$mid][0]                = strtolower($this->structure[$mid]->bytes);
                }
                
                $this->disposition[$mid][0]             = 'inline';
            }

            return;
        }
    }

?>
