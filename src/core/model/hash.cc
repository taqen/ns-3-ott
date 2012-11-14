/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Lawrence Livermore National Laboratory
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Peter D. Barnes, Jr. <pdbarnes@llnl.gov>
 */

#include "ns3/hash.h"
#include "ns3/log.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Hash");

/*************************************************
 **  class Hash
 ************************************************/

TypeId
Hash::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Hash");
  return tid;
}
  
Hash::Hash ()
{
  m_impl = Create <HashImplNS::Murmur3> ();
}

Hash::Hash (Ptr<HashImplementation> hp)
  : m_impl(hp)
{
}
  
  
/*************************************************
 **  class HashImplementation
 ************************************************/

uint64_t
HashImplementation::GetHash64  (const char * buffer, const size_t size)
{
  NS_LOG_WARN("64-bit hash requested, only 32-bit implementation available");
  return GetHash32 (buffer, size);
}

} // namespace ns3