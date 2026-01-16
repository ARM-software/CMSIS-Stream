/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        EventQueue.hpp
 * Description:  Interface for the event queue handling events in the CMSIS Stream Library
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2023-2025 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

/***************
 *
 * Event system
 *
 **************/

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "StreamNode.hpp"

#ifndef CG_TIME_STAMP_TYPE
#error "CG_TIME_STAMP_TYPE must be defined"
#endif

#ifndef CG_GET_TIME_STAMP
#error "CG_GET_TIME_STAMP must be defined"
#endif

namespace arm_cmsis_stream
{

/* Same computer */
struct LocalDestination {
	/* Destination node */
	StreamNode *dst;
	/* Destination port */
	int dstPort;
};

/* To network if supported by the application callback */
struct DistantDestination {
	int32_t src_node_id;
};

struct Message {
	std::variant<LocalDestination, DistantDestination> destination;
	/* Event */
	Event event;
	/* Timestamp */
	CG_TIME_STAMP_TYPE timestamp;
};

// In case of a threaded implementation
// an implementation of this API should be thread safe
class EventQueue
{
      public:
	using AppHandler = bool (*)(int src_node_id, void *data, Event &&evt);

	bool push(LocalDestination dest, Event &&evt)
	{
		Message msg{std::move(dest), std::move(evt), CG_GET_TIME_STAMP()};
		return (this->push(std::move(msg)));
	};

	bool push(DistantDestination dest, Event &&evt)
	{
		Message msg{std::move(dest), std::move(evt), CG_GET_TIME_STAMP()};
		return (this->push(std::move(msg)));
	};

	EventQueue() {};
	virtual ~EventQueue() {};

	virtual bool isEmpty() = 0;
	virtual void clear() = 0;

	// In case of a threaded implementation
	// this should sleep when no more any event are available
	// and wakeup when notified from push or end
	virtual void execute() = 0;

	// True if a request to end the event processing was made
	bool mustEnd() const noexcept
	{
		return (mustEnd_.load());
	};

	// True if a request to pause the event processing was made
	bool mustPause() const noexcept
	{
		return (mustPause_.load());
	};

	// In case of multi-threaded implementation
	// this should wakeup the thread
	// Signal a request to end the event processing
	virtual void end() noexcept
	{
		mustEnd_.store(true);
	};

	// Ask for a pause of the event queue processing
	// In case of multi-threaded implementation
	// this should wakeup the thread
	virtual void pause() noexcept
	{
		mustPause_.store(true);
	};

	// Resume the event queue processing queue
	void resume() noexcept
	{
		mustPause_.store(false);
	};

	
	// Used by queue implementations and EventOutput
	// when event sent to application in an asynchronous way
	// Should not be directly
	bool callAsyncHandler(int node_id, arm_cmsis_stream::Event &&evt)
	{
		if (mustEnd_) {
			return false; // Do not call the handler if we are ending
		}
		if (EventQueue::handlerReady_) {
			if (EventQueue::handler) {
				return (EventQueue::handler(node_id, EventQueue::handlerData,
							    std::move(evt)));
			}
			return false;
		}
		return false;
	};

	// Used by EventOutput and queue implementation
	// to send event to application in a synchronous way
	static bool callSyncHandler(int node_id, arm_cmsis_stream::Event &&evt)
	{
		if (EventQueue::handlerReady_) {
			if (EventQueue::handler) {
				return (EventQueue::handler(node_id, EventQueue::handlerData,
							    std::move(evt)));
			}
			return false;
		}
		return false;
	};

	// Set the application handler for events sent to application
	static void setHandler(void *data, AppHandler handler)
	{
		EventQueue::handlerData = data;
		EventQueue::handler = handler;
		EventQueue::handlerReady_ = true;
	};

      protected:
	// In case of a threaded implementation
	// this should wakeup the thread
	// Return false in case of queue overflow
	virtual bool push(Message &&message) = 0;

	static void *handlerData;
	static AppHandler handler;
	std::atomic<bool> mustEnd_ = false;
	std::atomic<bool> mustPause_ = false;
	static std::atomic<bool> handlerReady_;
};

class EventOutput
{
	EventOutput() = delete;

      public:
	EventOutput(EventQueue *queue) : cg_eventQueue(queue)
	{
	}

      protected:
	enum EventMode {
		kSync,
		kAsync
	};

	bool sendEventToAllNodes(Event &&evt, EventMode mode = kSync)
	{
		if ((mode == kAsync) && ((cg_eventQueue == nullptr) || cg_eventQueue->mustEnd())) {
			return false;
		}

		if (evt.event_id == kNoEvent) {
			return true; // No event to send
		}

		if (mNodes.size() == 1) {
			LocalDestination destination = mNodes[0];
			if (mode == kAsync) {
				// If async, we just push the event to the queue
				if (!cg_eventQueue->push(destination, std::move(evt))) {
					// If the queue is full, we return false
					return false;
				}
			} else {
				// If not async, we call the processEvent directly
				destination.dst->processEvent(destination.dstPort, std::move(evt));
			}
		} else {

			for (LocalDestination destination : mNodes) {
				if (mode == kAsync) {
					// If async, we just push the event to the queue
					if (!cg_eventQueue->push(destination, evt.clone())) {
						// If the queue is full, we return false
						return false;
					}
				} else {
					// If not async, we call the processEvent directly
					destination.dst->processEvent(destination.dstPort,
								      std::move(evt.clone()));
				}
			}
		}
		return true; // Event sent successfully
	};

      public:
	void subscribe(StreamNode &node, int dstPort = 0)
	{
		mNodes.push_back(LocalDestination{&node, dstPort});
	};

	template <typename... Args>
	void sendSync(enum cg_event_priority priority, uint32_t selector, Args &&...args)
	{
		sendCombinedValue(priority, kSync, selector, 0, std::forward<Args>(args)...);
	}

	template <typename... Args>
	bool sendAsync(enum cg_event_priority priority, uint32_t selector, Args &&...args)
	{
		return sendCombinedValue(priority, kAsync, selector, 0,
					 std::forward<Args>(args)...);
	}

	template <typename... Args>
	bool sendAsyncWithTTL(enum cg_event_priority priority, uint32_t selector, uint32_t ttl,
			      Args &&...args)
	{
		return sendCombinedValue(priority, kAsync, selector, ttl,
					 std::forward<Args>(args)...);
	}

	template <typename... Args> bool sendAsync(Event &&evt)
	{
		return sendEventToAllNodes(std::move(evt), kAsync);
	}

	template <typename... Args>
	static void sendSyncToApp(int node_id, enum cg_event_priority priority, uint32_t selector,
			   Args &&...args)
	{
		EventOutput::sendSyncToApp_(node_id, priority, selector, std::forward<Args>(args)...);
	}

	template <typename... Args>
	bool sendAsyncToApp(int node_id, enum cg_event_priority priority, uint32_t selector,
			    Args &&...args)
	{
		return sendAsyncToApp_(node_id, priority, selector, std::forward<Args>(args)...);
	}

      protected:
	template <typename... Args>
	bool sendCombinedValue(enum cg_event_priority priority, EventMode mode, uint32_t selector,
			       uint32_t ttl, Args &&...args)
	{
		// When more than one value
		// we create a combined event
		Event evt(selector, priority, std::forward<Args>(args)...);
		if (ttl != 0) {
			evt.setTTL(ttl);
		}
		return sendEventToAllNodes(std::move(evt), mode);
	};

	template <typename... Args>
	bool sendAsyncToApp_(int node_id, enum cg_event_priority priority, uint32_t selector,
			    Args &&...args)
	{

		if ((cg_eventQueue == nullptr) || cg_eventQueue->mustEnd()) {
			return false;
		}

		Event evt(selector, priority, std::forward<Args>(args)...);

		if (!cg_eventQueue->push(DistantDestination{node_id}, std::move(evt))) {
			// If the queue is full, we return false
			return false;
		}
		return true;
	};

	template <typename... Args>
	static bool sendSyncToApp_(int node_id, enum cg_event_priority priority, uint32_t selector,
			   Args &&...args)
	{

		Event evt(selector, priority, std::forward<Args>(args)...);
		return EventQueue::callSyncHandler(node_id, std::move(evt));
		
	};

	std::vector<LocalDestination> mNodes;
	EventQueue *cg_eventQueue = nullptr;
};

} // namespace arm_cmsis_stream
