/*
 * Gate.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SOURCES_WEBSOCKET_WEBSCOKET_H_
#define SOURCES_WEBSOCKET_WEBSCOKET_H_


enum WebSocketStatus {
	INIT = 0,
	CONNECTING = 1,
	OPEN = 2,
	FAILED = 3,
	CLOSE = 4
};


#endif /* SOURCES_WEBSOCKET_WEBSCOKET_H_ */
