import data.buffer

namespace tactic.debug

meta constant handle : Type

meta constant mk_handle (token : string) : tactic handle
meta constant handle.publish_raw (h : handle) (msg : char_buffer) : tactic unit
meta constant handle.pause (h : handle) : tactic unit

meta def handle.publish (h : handle) (msg : string) : tactic unit := h.publish_raw msg.to_char_buffer

end tactic.debug