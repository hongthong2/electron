// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/atom_message_filter.h"

#include "content/browser/web_contents/web_contents_impl.h"  // nogncheck
#include "content/common/widget_messages.h"

namespace electron {

AtomMessageFilter::AtomMessageFilter(content::WebContents* web_contents)
    : BrowserMessageFilter(WidgetMsgStart), web_contents_(web_contents) {}

AtomMessageFilter::~AtomMessageFilter() {}

bool AtomMessageFilter::OnMessageReceived(const IPC::Message& message) {
  IPC_BEGIN_MESSAGE_MAP(AtomMessageFilter, message)
    IPC_MESSAGE_HANDLER_GENERIC(WidgetHostMsg_SetCursor,
                                ForwardMessage(message))
  IPC_END_MESSAGE_MAP()

  return false;
}

void ForwardMessageOnUIThread(const IPC::Message& message,
                              content::WebContentsImpl* web_contents_impl) {
  web_contents_impl->OnMessageReceived(web_contents_impl->GetMainFrame(),
                                       message);
}

void AtomMessageFilter::ForwardMessage(const IPC::Message& message) {
  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents_);
  base::PostTaskWithTraits(
      FROM_HERE, {content::BrowserThread::UI},
      base::BindOnce(&ForwardMessageOnUIThread, message, web_contents_impl));
}

}  // namespace electron