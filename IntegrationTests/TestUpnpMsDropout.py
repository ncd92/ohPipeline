#!/usr/bin/env python
"""TestUpnpMsDropout - test for dropout from UPnP media server served tracks.

Parameters:
    arg#1 - Sender DUT
    arg#2 - Receiver/Repeater DUT (None = not present)
    arg#3 - Receiver/SlaveDUT  (None = not present)
    arg#4 - Test duration (secs) or 'forever'
    arg#5 - Media server to source media from
    arg#6 - Playlist name

    Use 'local' for internal SoftPlayer on loopback for DUTs

Verifies UPnP media server served audio played by the DUT does not suffer from audio dropout.
Additionally checks for dropout on downstream songcast receiver(s)
"""

import _FunctionalTest
import _BaseDropout                   as BASE
import Upnp.ControlPoints.MediaServer as Server
import os
import sys

kAudioRoot = os.path.join( _FunctionalTest.audioDir, 'LRTones/' )
kTrackList = os.path.join( kAudioRoot, 'TrackList.xml' )


class TestUpnpMsDropout( BASE.BaseDropout ):
    """Test dropout on UPnP media server served tracks"""

    """Test playing of locally served tracks"""

    def __init__( self ):
        """Constructor - initialise base class"""
        BASE.BaseDropout.__init__( self )
        self.doc = __doc__
        self.server = None

    def Test( self, args ):
        """Test dropout on UPnP media server served tracks"""
        serverName   = ''
        playlistName = ''
        try:
            serverName   = args[5]
            playlistName = args[6]
        except:
            print '\n', __doc__, '\n'
            self.log.Abort( '', 'Invalid arguments %s' % (str( args )) )

        server = Server.MediaServer( serverName )
        if not server.device:
            log.Abort( serverName, 'Not available' )
        self.tracks = server.GetPlaylist( playlistName )
        server.Shutdown()

        BASE.BaseDropout.Test( self, args )

    def Cleanup( self ):
        """Perform post-test cleanup"""
        BASE.BaseDropout.Cleanup( self )
        if self.server:
            self.server.Shutdown()

            
if __name__ == '__main__':
    
    BASE.Run( sys.argv )
    